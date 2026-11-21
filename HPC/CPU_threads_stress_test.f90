! Here is a Fortran program to stress test the CPU, utilizing all cores and threads with OpenMP, and reporting the time consumed
! You can change the max_iteration number on line 20
! Compile it with OpenMP support:
!         gfortran -O3 -fopenmp -o CPU_threads_stress_test CPU_threads_stress_test.f90
! Run the program:
!        ./cpu_stress_test
! Install the latest gfortran release by updating to the latest GCC release. (GCC13 is the latest stable release as of January 2025)
! GNU Fortran is also accessible after installing GNU Octave, if you are using Microsoft Windows
! You can also download MinGW-w64 from: https://winlibs.com that has gfortran.exe in \mingw64\bin


PROGRAM CPUThreadsStressTest
  USE, INTRINSIC :: iso_fortran_env, ONLY: int64, real64
  USE omp_lib
  IMPLICIT NONE

  INTEGER(int64) :: max_iterations, i
  INTEGER(int64) :: start_count, end_count, clock_rate
  INTEGER :: num_threads, arg_status
  REAL(real64) :: result, elapsed_seconds
  CHARACTER(len=64) :: arg

  max_iterations = 10000000000_int64

  IF (command_argument_count() >= 1) THEN
    CALL get_command_argument(1, arg)
    READ(arg, *, IOSTAT=arg_status) max_iterations
    IF (arg_status /= 0 .OR. max_iterations < 1_int64) THEN
      PRINT *, 'Invalid iteration count'
      STOP 1
    END IF
  END IF

  result = 0.0_real64
  num_threads = 0

  PRINT *, 'Starting parallel CPU stress test...'
  PRINT *, 'Iterations: ', max_iterations
  PRINT *, 'Maximum threads: ', omp_get_max_threads()

  CALL system_clock(count_rate=clock_rate)
  IF (clock_rate <= 0_int64) ERROR STOP 'Clock unavailable'
  CALL system_clock(count=start_count)

  !$OMP PARALLEL DEFAULT(NONE) &
  !$OMP SHARED(max_iterations,num_threads) &
  !$OMP PRIVATE(i) REDUCTION(+:result)

  !$OMP SINGLE
  num_threads = omp_get_num_threads()
  !$OMP END SINGLE

  !$OMP DO SCHEDULE(STATIC)
  DO i = 1_int64, max_iterations
    result = result + SIN(REAL(i,real64)) * COS(REAL(i,real64))
  END DO
  !$OMP END DO
  !$OMP END PARALLEL

  CALL system_clock(count=end_count)

  elapsed_seconds = REAL(end_count-start_count,real64) / &
                    REAL(clock_rate,real64)

  PRINT *, 'Stress test completed.'
  PRINT *, 'Threads used: ', num_threads
  PRINT *, 'Result: ', result
  PRINT *, 'Elapsed time (seconds): ', elapsed_seconds

END PROGRAM CPUThreadsStressTest
