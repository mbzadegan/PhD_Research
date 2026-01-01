! Compare Recursive vs. Dynamic Programming CPU time
! Calculate the n-th Fibonacci number with two methods, the Recursive vs. the Dynamic algorithm

! gfortran Compare_Recursive_Dynamic.f90 -o fib
! ---------------------------------------------------------------------------------



program fibonacci_comparison
    implicit none
    integer :: n
    integer(kind=8) :: result_recursive, result_dp
    real :: t1, t2, t_recursive, t_dp

    ! Ask the user for the Fibonacci index
    print *, "Enter n (index of the Fibonacci number):"
    read *, n

    if (n < 0) then
        print *, "Error: n must be non-negative."
        stop
    end if

    ! -------------------------------
    ! Method 1: Recursive (Generic)
    ! -------------------------------
    call cpu_time(t1)
    result_recursive = fib_recursive(n)
    call cpu_time(t2)
    t_recursive = t2 - t1

    ! -------------------------------
    ! Method 2: Dynamic Programming
    ! -------------------------------
    call cpu_time(t1)
    result_dp = fib_dp(n)
    call cpu_time(t2)
    t_dp = t2 - t1

    ! -------------------------------
    ! Results
    ! -------------------------------
    print *, "-------------------------------------------"
    print *, "Results:"
    print *, "Recursive Fibonacci(", n, ") = ", result_recursive
    print *, "Dynamic Programming Fibonacci(", n, ") = ", result_dp
    print *, "-------------------------------------------"
    print *
    print *, "Time (recursive): ", t_recursive, " seconds."
    print *, "Time (DP):        ", t_dp,        " seconds."
    print *, "-------------------------------------------"

contains

    ! =============================================
    ! Generic Recursive Fibonacci (Exponential time)
    ! =============================================
    recursive function fib_recursive(n) result(f)
        integer, intent(in) :: n
        integer(kind=8) :: f

        if (n == 0) then
            f = 0
        elseif (n == 1) then
            f = 1
        else
            f = fib_recursive(n-1) + fib_recursive(n-2)
        end if
    end function fib_recursive

    ! =============================================
    ! Dynamic Programming Fibonacci (O(n))
    ! =============================================
    function fib_dp(n) result(f)
        integer, intent(in) :: n
        integer(kind=8) :: f
        integer(kind=8), allocatable :: dp(:)
        integer :: i

        allocate(dp(0:n))

        dp(0) = 0
        if (n > 0) dp(1) = 1

        do i = 2, n
            dp(i) = dp(i-1) + dp(i-2)
        end do

        f = dp(n)
        deallocate(dp)
    end function fib_dp

end program fibonacci_comparison
