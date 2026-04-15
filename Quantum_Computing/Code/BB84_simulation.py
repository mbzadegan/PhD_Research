#!/usr/bin/env python3
"""
bb84_simulation.py

An educational BB84 quantum key distribution simulation with:
- explicit qubit state vectors
- random basis preparation and measurement
- optional intercept-resend eavesdropper
- optional channel noise
- sifting
- QBER estimation
- simple parity-based error reconciliation
- privacy amplification

Requires:
    Python 3.10+
    numpy

Run:
    python bb84_simulation.py

Optional:
    python bb84_simulation.py --n 4096 --eve 0.25 --noise 0.01 --sample 0.2
"""

from __future__ import annotations

import argparse
import hashlib
import math
import secrets
import sys
from dataclasses import dataclass
from typing import List, Sequence, Tuple

import numpy as np


# =========================
# Linear algebra primitives
# =========================

SQRT2_INV = 1.0 / math.sqrt(2.0)

KET_0 = np.array([1.0 + 0.0j, 0.0 + 0.0j], dtype=np.complex128)
KET_1 = np.array([0.0 + 0.0j, 1.0 + 0.0j], dtype=np.complex128)
KET_PLUS = np.array([SQRT2_INV + 0.0j, SQRT2_INV + 0.0j], dtype=np.complex128)
KET_MINUS = np.array([SQRT2_INV + 0.0j, -SQRT2_INV + 0.0j], dtype=np.complex128)

H = np.array([[SQRT2_INV, SQRT2_INV],
              [SQRT2_INV, -SQRT2_INV]], dtype=np.complex128)

X = np.array([[0.0, 1.0],
              [1.0, 0.0]], dtype=np.complex128)

Z = np.array([[1.0, 0.0],
              [0.0, -1.0]], dtype=np.complex128)

I = np.eye(2, dtype=np.complex128)


def normalize_state(state: np.ndarray) -> np.ndarray:
    norm = np.linalg.norm(state)
    if norm == 0:
        raise ValueError("State vector has zero norm.")
    return state / norm


def basis_vectors(basis: str) -> Tuple[np.ndarray, np.ndarray]:
    """
    Return the measurement / encoding basis vectors.

    basis='Z' -> {|0>, |1>}
    basis='X' -> {|+>, |->}
    """
    if basis == "Z":
        return KET_0, KET_1
    if basis == "X":
        return KET_PLUS, KET_MINUS
    raise ValueError(f"Unknown basis: {basis}")


def bit_to_state(bit: int, basis: str) -> np.ndarray:
    zero_state, one_state = basis_vectors(basis)
    if bit == 0:
        return zero_state.copy()
    if bit == 1:
        return one_state.copy()
    raise ValueError(f"Bit must be 0 or 1, got {bit}")


def born_measurement_probabilities(state: np.ndarray, basis: str) -> Tuple[float, float]:
    """
    Return probabilities of outcomes 0 and 1 when measuring `state` in `basis`.
    """
    v0, v1 = basis_vectors(basis)
    p0 = float(np.abs(np.vdot(v0, state)) ** 2)
    p1 = float(np.abs(np.vdot(v1, state)) ** 2)

    # Numerical guard
    p0 = max(0.0, min(1.0, p0))
    p1 = max(0.0, min(1.0, p1))

    total = p0 + p1
    if total <= 0:
        raise ValueError("Invalid probabilities: both zero.")
    p0 /= total
    p1 /= total
    return p0, p1


def measure_state(state: np.ndarray, basis: str, rng: np.random.Generator) -> Tuple[int, np.ndarray]:
    """
    Projectively measure a qubit in basis Z or X.
    Returns (classical_bit, post_measurement_state).
    """
    p0, p1 = born_measurement_probabilities(state, basis)
    outcome = int(rng.choice([0, 1], p=[p0, p1]))
    collapsed = bit_to_state(outcome, basis)
    return outcome, collapsed


def apply_random_channel_noise(
    state: np.ndarray,
    noise_probability: float,
    rng: np.random.Generator
) -> np.ndarray:
    """
    Apply a simple random Pauli noise model:
      - with probability noise_probability, apply one of X, Z, XZ uniformly
      - otherwise apply identity
    """
    if noise_probability < 0.0 or noise_probability > 1.0:
        raise ValueError("noise_probability must be in [0, 1].")

    if rng.random() >= noise_probability:
        return state

    op = rng.choice(["X", "Z", "XZ"])
    if op == "X":
        return normalize_state(X @ state)
    if op == "Z":
        return normalize_state(Z @ state)
    return normalize_state(X @ (Z @ state))


# =========================
# BB84 protocol components
# =========================

@dataclass
class BB84Config:
    n_qubits: int = 2048
    eve_intercept_probability: float = 0.0
    channel_noise_probability: float = 0.0
    test_sample_fraction: float = 0.2
    qber_abort_threshold: float = 0.11
    reconciliation_block_size: int = 16
    rng_seed: int | None = None


@dataclass
class TransmissionRecord:
    alice_bits: np.ndarray
    alice_bases: np.ndarray
    bob_bits: np.ndarray
    bob_bases: np.ndarray
    eve_intercepted: np.ndarray
    sift_indices: np.ndarray


@dataclass
class ParameterEstimationResult:
    test_indices_local: np.ndarray
    key_indices_local: np.ndarray
    qber_estimate: float
    test_errors: int
    test_size: int


@dataclass
class ReconciliationResult:
    alice_reconciled: np.ndarray
    bob_reconciled: np.ndarray
    estimated_residual_errors: int
    disclosed_parity_bits: int


@dataclass
class FinalKeyResult:
    final_alice_key: np.ndarray
    final_bob_key: np.ndarray
    final_key_length: int
    matched: bool


class BB84Protocol:
    def __init__(self, config: BB84Config):
        if config.n_qubits <= 0:
            raise ValueError("n_qubits must be positive.")
        if not (0.0 <= config.eve_intercept_probability <= 1.0):
            raise ValueError("eve_intercept_probability must be in [0, 1].")
        if not (0.0 <= config.channel_noise_probability <= 1.0):
            raise ValueError("channel_noise_probability must be in [0, 1].")
        if not (0.0 < config.test_sample_fraction < 1.0):
            raise ValueError("test_sample_fraction must be in (0, 1).")
        if not (0.0 <= config.qber_abort_threshold <= 1.0):
            raise ValueError("qber_abort_threshold must be in [0, 1].")
        if config.reconciliation_block_size <= 0:
            raise ValueError("reconciliation_block_size must be positive.")

        self.config = config
        self.rng = np.random.default_rng(config.rng_seed)

    def random_bits(self, n: int) -> np.ndarray:
        return self.rng.integers(0, 2, size=n, dtype=np.int8)

    def random_bases(self, n: int) -> np.ndarray:
        return self.rng.choice(np.array(["Z", "X"]), size=n)

    def prepare_alice_states(self, bits: np.ndarray, bases: np.ndarray) -> List[np.ndarray]:
        return [bit_to_state(int(bit), str(basis)) for bit, basis in zip(bits, bases)]

    def eve_intercept_resend(
        self,
        state: np.ndarray
    ) -> Tuple[np.ndarray, bool]:
        """
        Eve intercepts with a given probability.
        If intercepting, she measures in a random basis and resends
        the collapsed state corresponding to her measurement result.
        """
        intercepted = self.rng.random() < self.config.eve_intercept_probability
        if not intercepted:
            return state, False

        eve_basis = str(self.rng.choice(["Z", "X"]))
        eve_bit, resent_state = measure_state(state, eve_basis, self.rng)
        _ = eve_bit  # kept for clarity; could be logged if desired
        return resent_state, True

    def run_quantum_transmission(self) -> TransmissionRecord:
        n = self.config.n_qubits

        alice_bits = self.random_bits(n)
        alice_bases = self.random_bases(n)
        alice_states = self.prepare_alice_states(alice_bits, alice_bases)

        bob_bases = self.random_bases(n)
        bob_bits = np.zeros(n, dtype=np.int8)
        eve_intercepted = np.zeros(n, dtype=bool)

        for i, state in enumerate(alice_states):
            current_state = state.copy()

            current_state, intercepted = self.eve_intercept_resend(current_state)
            eve_intercepted[i] = intercepted

            current_state = apply_random_channel_noise(
                current_state,
                self.config.channel_noise_probability,
                self.rng
            )

            bob_bit, _ = measure_state(current_state, str(bob_bases[i]), self.rng)
            bob_bits[i] = bob_bit

        sift_mask = (alice_bases == bob_bases)
        sift_indices = np.nonzero(sift_mask)[0]

        return TransmissionRecord(
            alice_bits=alice_bits,
            alice_bases=alice_bases,
            bob_bits=bob_bits,
            bob_bases=bob_bases,
            eve_intercepted=eve_intercepted,
            sift_indices=sift_indices,
        )

    def sift_keys(self, record: TransmissionRecord) -> Tuple[np.ndarray, np.ndarray]:
        alice_sifted = record.alice_bits[record.sift_indices].copy()
        bob_sifted = record.bob_bits[record.sift_indices].copy()
        return alice_sifted, bob_sifted

    def estimate_qber(
        self,
        alice_sifted: np.ndarray,
        bob_sifted: np.ndarray
    ) -> ParameterEstimationResult:
        if len(alice_sifted) != len(bob_sifted):
            raise ValueError("Sifted keys must have the same length.")

        m = len(alice_sifted)
        if m == 0:
            raise ValueError("No sifted key bits available.")

        sample_size = max(1, int(self.config.test_sample_fraction * m))
        all_indices = np.arange(m)
        test_indices = self.rng.choice(all_indices, size=sample_size, replace=False)
        key_indices = np.setdiff1d(all_indices, test_indices, assume_unique=False)

        test_errors = int(np.sum(alice_sifted[test_indices] != bob_sifted[test_indices]))
        qber = test_errors / sample_size

        return ParameterEstimationResult(
            test_indices_local=np.sort(test_indices),
            key_indices_local=np.sort(key_indices),
            qber_estimate=qber,
            test_errors=test_errors,
            test_size=sample_size,
        )

    @staticmethod
    def parity(bits: np.ndarray) -> int:
        if len(bits) == 0:
            return 0
        return int(np.bitwise_xor.reduce(bits))

    def binary_search_error_correct(
        self,
        alice_block: np.ndarray,
        bob_block: np.ndarray
    ) -> Tuple[np.ndarray, int]:
        """
        Recursively locate a single error in a block if parity differs.
        Returns corrected bob_block and number of disclosed parity bits.
        """
        disclosed = 1  # one parity comparison for this block
        if len(alice_block) != len(bob_block):
            raise ValueError("Blocks must have same length.")

        if self.parity(alice_block) == self.parity(bob_block):
            return bob_block.copy(), disclosed

        if len(alice_block) == 1:
            corrected = bob_block.copy()
            corrected[0] ^= 1
            return corrected, disclosed

        mid = len(alice_block) // 2
        left_a, right_a = alice_block[:mid], alice_block[mid:]
        left_b, right_b = bob_block[:mid], bob_block[mid:]

        disclosed += 1
        if self.parity(left_a) != self.parity(left_b):
            corrected_left, extra = self.binary_search_error_correct(left_a, left_b)
            disclosed += extra
            return np.concatenate([corrected_left, right_b]), disclosed
        else:
            corrected_right, extra = self.binary_search_error_correct(right_a, right_b)
            disclosed += extra
            return np.concatenate([left_b, corrected_right]), disclosed

    def reconcile_keys(
        self,
        alice_key: np.ndarray,
        bob_key: np.ndarray
    ) -> ReconciliationResult:
        """
        Very simple educational reconciliation:
        - divide into blocks
        - compare parity
        - if mismatch, recursively binary-search a single error

        This is not a complete real-world authenticated reconciliation protocol.
        """
        if len(alice_key) != len(bob_key):
            raise ValueError("Keys must have same length.")

        block_size = self.config.reconciliation_block_size
        n = len(alice_key)

        bob_corrected = bob_key.copy()
        disclosed_parity_bits = 0

        for start in range(0, n, block_size):
            end = min(start + block_size, n)
            a_block = alice_key[start:end]
            b_block = bob_corrected[start:end]

            corrected_block, disclosed = self.binary_search_error_correct(a_block, b_block)
            bob_corrected[start:end] = corrected_block
            disclosed_parity_bits += disclosed

        residual_errors = int(np.sum(alice_key != bob_corrected))
        return ReconciliationResult(
            alice_reconciled=alice_key.copy(),
            bob_reconciled=bob_corrected,
            estimated_residual_errors=residual_errors,
            disclosed_parity_bits=disclosed_parity_bits,
        )

    @staticmethod
    def binary_entropy(p: float) -> float:
        if p <= 0.0 or p >= 1.0:
            return 0.0
        return -p * math.log2(p) - (1.0 - p) * math.log2(1.0 - p)

    def privacy_amplification(
        self,
        alice_key: np.ndarray,
        bob_key: np.ndarray,
        qber_estimate: float,
        disclosed_parity_bits: int
    ) -> FinalKeyResult:
        """
        Use a random binary matrix (Toeplitz-like via seeded PRNG) for privacy amplification.

        A simple heuristic for final key length:
            final_len ~ n * max(0, 1 - 2*h(qber)) - leak_ec - safety_margin
        """
        if len(alice_key) != len(bob_key):
            raise ValueError("Keys must have same length.")

        n = len(alice_key)
        hq = self.binary_entropy(qber_estimate)

        safety_margin = 32
        final_len = int(n * max(0.0, 1.0 - 2.0 * hq) - disclosed_parity_bits - safety_margin)
        final_len = max(0, min(final_len, n))

        if final_len == 0:
            return FinalKeyResult(
                final_alice_key=np.array([], dtype=np.int8),
                final_bob_key=np.array([], dtype=np.int8),
                final_key_length=0,
                matched=True,
            )

        seed = secrets.token_bytes(32)
        prng = np.random.default_rng(int.from_bytes(seed[:8], "big", signed=False))

        # Random binary matrix of shape (final_len, n)
        compression_matrix = prng.integers(0, 2, size=(final_len, n), dtype=np.int8)

        final_alice = (compression_matrix @ alice_key) % 2
        final_bob = (compression_matrix @ bob_key) % 2

        matched = bool(np.array_equal(final_alice, final_bob))

        return FinalKeyResult(
            final_alice_key=final_alice.astype(np.int8),
            final_bob_key=final_bob.astype(np.int8),
            final_key_length=final_len,
            matched=matched,
        )

    @staticmethod
    def bit_array_to_str(bits: np.ndarray, max_len: int = 128) -> str:
        s = "".join(str(int(b)) for b in bits[:max_len])
        if len(bits) > max_len:
            s += "..."
        return s

    @staticmethod
    def digest_bits(bits: np.ndarray) -> str:
        packed = "".join(str(int(b)) for b in bits).encode("utf-8")
        return hashlib.sha256(packed).hexdigest()

    def run(self) -> None:
        record = self.run_quantum_transmission()
        alice_sifted, bob_sifted = self.sift_keys(record)

        if len(alice_sifted) == 0:
            print("Abort: no sifted bits available.")
            return

        pe = self.estimate_qber(alice_sifted, bob_sifted)

        alice_key = alice_sifted[pe.key_indices_local]
        bob_key = bob_sifted[pe.key_indices_local]

        intercepted_count = int(np.sum(record.eve_intercepted))
        raw_qber_full_sifted = float(np.mean(alice_sifted != bob_sifted)) if len(alice_sifted) else 0.0

        print("\n=== BB84 Simulation Report ===")
        print(f"Qubits sent by Alice:              {self.config.n_qubits}")
        print(f"Eve intercept probability:         {self.config.eve_intercept_probability:.4f}")
        print(f"Channel noise probability:         {self.config.channel_noise_probability:.4f}")
        print(f"Actual transmissions intercepted:  {intercepted_count}")
        print(f"Sifted key length:                 {len(alice_sifted)}")
        print(f"Raw sifted QBER (full sifted set): {raw_qber_full_sifted:.4%}")
        print(f"Test sample size:                  {pe.test_size}")
        print(f"Observed test errors:              {pe.test_errors}")
        print(f"Estimated QBER from test sample:   {pe.qber_estimate:.4%}")
        print(f"Abort threshold:                   {self.config.qber_abort_threshold:.4%}")

        if pe.qber_estimate > self.config.qber_abort_threshold:
            print("\nProtocol ABORTED: QBER too high.")
            return

        print("\nParameter estimation passed.")

        rec = self.reconcile_keys(alice_key, bob_key)
        print("\n--- Reconciliation ---")
        print(f"Key length after test-bit removal: {len(alice_key)}")
        print(f"Disclosed parity bits:             {rec.disclosed_parity_bits}")
        print(f"Residual mismatches after EC:      {rec.estimated_residual_errors}")

        final_res = self.privacy_amplification(
            rec.alice_reconciled,
            rec.bob_reconciled,
            pe.qber_estimate,
            rec.disclosed_parity_bits,
        )

        print("\n--- Privacy Amplification ---")
        print(f"Final key length:                  {final_res.final_key_length}")
        print(f"Alice/Bob final keys match:        {final_res.matched}")

        if final_res.final_key_length > 0:
            print("\nAlice final key preview:")
            print(self.bit_array_to_str(final_res.final_alice_key))
            print("\nBob final key preview:")
            print(self.bit_array_to_str(final_res.final_bob_key))
            print("\nAlice SHA-256(final key bits):")
            print(self.digest_bits(final_res.final_alice_key))
            print("\nBob SHA-256(final key bits):")
            print(self.digest_bits(final_res.final_bob_key))
        else:
            print("\nNo final secret key could be distilled under current parameters.")


# =========================
# CLI
# =========================

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Advanced BB84 protocol simulation with noise, Eve, QBER estimation, reconciliation, and privacy amplification."
    )
    parser.add_argument("--n", type=int, default=2048, help="Number of qubits transmitted by Alice.")
    parser.add_argument("--eve", type=float, default=0.0, help="Probability that Eve intercepts each qubit.")
    parser.add_argument("--noise", type=float, default=0.0, help="Channel noise probability.")
    parser.add_argument("--sample", type=float, default=0.2, help="Fraction of sifted bits revealed for QBER estimation.")
    parser.add_argument("--qber-threshold", type=float, default=0.11, help="Abort threshold for QBER.")
    parser.add_argument("--block-size", type=int, default=16, help="Reconciliation block size.")
    parser.add_argument("--seed", type=int, default=None, help="Random seed for reproducibility.")
    return parser


def main() -> int:
    try:
        args = build_parser().parse_args()

        config = BB84Config(
            n_qubits=args.n,
            eve_intercept_probability=args.eve,
            channel_noise_probability=args.noise,
            test_sample_fraction=args.sample,
            qber_abort_threshold=args.qber_threshold,
            reconciliation_block_size=args.block_size,
            rng_seed=args.seed,
        )

        protocol = BB84Protocol(config)
        protocol.run()
        return 0

    except KeyboardInterrupt:
        print("\nInterrupted by user.", file=sys.stderr)
        return 130
    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
