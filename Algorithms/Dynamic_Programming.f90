! Matrix Chain Multiplication
! We’re given a sequence of matrices 𝐴1, 𝐴2, …, 𝐴𝑛 where matrix 𝐴𝑖 has dimensions 𝑝(𝑖-1)×𝑝𝑖
! We want to find the order of multiplying them that minimizes the number of scalar multiplications.


program matrix_chain
    implicit none
    integer, parameter :: n = 6
    integer :: p(n+1)
    integer :: m(n,n), s(n,n)
    integer :: i, j, k, L, q

    ! Example dimensions: A1(30x35), A2(35x15), A3(15x5), A4(5x10), A5(10x20), A6(20x25)
    p = (/30, 35, 15, 5, 10, 20, 25/)

    ! Initialize cost matrix
    m = 0
    s = 0

    ! L is the chain length
    do L = 2, n
        do i = 1, n - L + 1
            j = i + L - 1
            m(i,j) = huge(0)   ! initialize to infinity
            do k = i, j - 1
                q = m(i,k) + m(k+1,j) + p(i-1+1)*p(k+1)*p(j+1)
                if (q < m(i,j)) then
                    m(i,j) = q
                    s(i,j) = k
                end if
            end do
        end do
    end do

    print *, "Minimum number of multiplications: ", m(1,n)
    print *, "Optimal parenthesization:"
    call print_optimal_parens(s, 1, n)
    print *

contains

    recursive subroutine print_optimal_parens(s, i, j)
        integer, intent(in) :: s(:,:), i, j
        if (i == j) then
            write(*,'(A,I0)', advance="no") "A", i
        else
            write(*,'(A)', advance="no") "("
            call print_optimal_parens(s, i, s(i,j))
            call print_optimal_parens(s, s(i,j)+1, j)
            write(*,'(A)', advance="no") ")"
        end if
    end subroutine print_optimal_parens

end program matrix_chain
