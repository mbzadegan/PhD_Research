!--------------------------------------------------------------
! Bellman-Ford Algorithm (Single Source Shortest Paths)
! From CLRS Chapter 24.1
!--------------------------------------------------------------

program bellman_ford
    implicit none
    integer, parameter :: INF = 99999
    integer, parameter :: V = 5, E = 8
    integer :: i, j, src
    logical :: has_negative_cycle

    ! Each edge: (u, v, w)
    integer :: u(E), v(E), w(E)
    integer :: dist(V)

    ! Define a sample graph (from CLRS Figure 24.4)
    data (u(i), v(i), w(i), i=1,E) / &
        1, 2, 6, &
        1, 3, 7, &
        2, 3, 8, &
        2, 4, 5, &
        2, 5, -4, &
        3, 4, -3, &
        4, 2, -2, &
        5, 4, 7 /

    src = 1
    call bellman_ford_algorithm(V, E, u, v, w, src, dist, has_negative_cycle)

    if (has_negative_cycle) then
        print *, "Graph contains a negative-weight cycle!"
    else
        print *, "Vertex   Distance from Source (", src, "):"
        do i = 1, V
            if (dist(i) == INF) then
                print '(I3, 5X, A)', i, "INF"
            else
                print '(I3, 5X, I5)', i, dist(i)
            end if
        end do
    end if

contains

    subroutine bellman_ford_algorithm(V, E, u, v, w, src, dist, has_neg)
        integer, intent(in) :: V, E, src
        integer, intent(in) :: u(E), v(E), w(E)
        integer, intent(out) :: dist(V)
        logical, intent(out) :: has_neg
        integer :: i, j

        ! Step 1: Initialize distances
        do i = 1, V
            dist(i) = INF
        end do
        dist(src) = 0

        ! Step 2: Relax all edges V-1 times
        do i = 1, V-1
            do j = 1, E
                if (dist(u(j)) /= INF .and. dist(u(j)) + w(j) < dist(v(j))) then
                    dist(v(j)) = dist(u(j)) + w(j)
                end if
            end do
        end do

        ! Step 3: Check for negative-weight cycles
        has_neg = .false.
        do j = 1, E
            if (dist(u(j)) /= INF .and. dist(u(j)) + w(j) < dist(v(j))) then
                has_neg = .true.
                exit
            end if
        end do
    end subroutine bellman_ford_algorithm

end program bellman_ford
