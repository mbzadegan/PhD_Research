! Longest Common Subsequence (LCS)
! Find the longest sequence that is a subsequence of both.


program lcs_problem
    implicit none
    character(len=*), parameter :: X = "ABCBDAB"
    character(len=*), parameter :: Y = "BDCABA"
    integer, parameter :: m = len(X), n = len(Y)
    integer :: c(0:m,0:n)
    character(len=1) :: b(m,n)
    integer :: i, j

    ! Initialize c with zeros
    c = 0

    ! Build the DP table
    do i = 1, m
        do j = 1, n
            if (X(i:i) == Y(j:j)) then
                c(i,j) = c(i-1,j-1) + 1
                b(i,j) = "D"   ! diagonal (match)
            else if (c(i-1,j) >= c(i,j-1)) then
                c(i,j) = c(i-1,j)
                b(i,j) = "U"   ! up
            else
                c(i,j) = c(i,j-1)
                b(i,j) = "L"   ! left
            end if
        end do
    end do

    print *, "Length of LCS = ", c(m,n)
    print *, "LCS: "
    call print_lcs(b, X, m, n)
    print *

contains

    recursive subroutine print_lcs(b, X, i, j)
        character(len=1), intent(in) :: b(:,:)
        character(len=*), intent(in) :: X
        integer, intent(in) :: i, j
        if (i == 0 .or. j == 0) then
            return
        else if (b(i,j) == "D") then
            call print_lcs(b, X, i-1, j-1)
            write(*,'(A)', advance="no") X(i:i)
        else if (b(i,j) == "U") then
            call print_lcs(b, X, i-1, j)
        else
            call print_lcs(b, X, i, j-1)
        end if
    end subroutine print_lcs

end program lcs_problem
