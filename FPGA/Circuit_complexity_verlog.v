// File: circuit_complexity_demo.v

module xor_gate (
    input wire a,
    input wire b,
    output wire y
);
    // XOR has known small circuit complexity
    assign y = a ^ b;
endmodule


module majority3 (
    input wire a,
    input wire b,
    input wire c,
    output wire y
);
    // Majority outputs one if at least two inputs are one
    assign y = (a & b) | (a & c) | (b & c);
endmodule


module testbench;
    reg a, b, c;
    wire xor_out;
    wire maj_out;

    xor_gate u1 (
        .a(a),
        .b(b),
        .y(xor_out)
    );

    majority3 u2 (
        .a(a),
        .b(b),
        .c(c),
        .y(maj_out)
    );

    initial begin
        $display("A B C | XOR | MAJ");
        $display("---------------");

        for (int i = 0; i < 8; i = i + 1) begin
            {a, b, c} = i[2:0];
            #1;
            $display("%b %b %b |  %b  |  %b", a, b, c, xor_out, maj_out);
        end

        $finish;
    end
endmodule
