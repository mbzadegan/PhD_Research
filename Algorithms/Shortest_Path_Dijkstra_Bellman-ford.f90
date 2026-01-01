!===========================================================
! Single-Source Shortest Path in Weighted Graph
! Dijkstra vs Bellman-Ford (Fortran 90+)
!===========================================================

module graph_module
    implicit none
    integer, parameter :: INF = 2147483647
contains

    !-------------------------------------------------------
    ! Bellman-Ford Algorithm
    !-------------------------------------------------------
    subroutine bellman_ford(n, m, u, v, w, source, dist, has_neg_cycle)
        implicit none
        integer, intent(in) :: n, m, source
        integer, intent(in) :: u(:), v(:), w(:)
        integer, intent(out) :: dist(:)
        logical, intent(out) :: has_neg_cycle

        integer :: i, j

        dist = INF
        dist(source) = 0

        ! Relax edges for n-1 iterations
        do i = 1, n - 1
            do j = 1, m
                if (dist(u(j)) /= INF .and. dist(u(j)) + w(j) < dist(v(j))) then
                    dist(v(j)) = dist(u(j)) + w(j)
                end if
            end do
        end do

        ! Check for negative cycle
        has_neg_cycle = .false.
        do j = 1, m
            if (dist(u(j)) /= INF .and. dist(u(j)) + w(j) < dist(v(j))) then
                has_neg_cycle = .true.
                return
            end if
        end do
    end subroutine bellman_ford


    !-------------------------------------------------------
    ! Dijkstra Algorithm (simple O(V²) version)
    !-------------------------------------------------------
    subroutine dijkstra(n, adj_head, adj_next, adj_v, adj_w, source, dist)
        implicit none
        integer, intent(in) :: n, source
        integer, intent(in) :: adj_head(:), adj_next(:), adj_v(:), adj_w(:)
        integer, intent(out) :: dist(:)

        logical :: visited(n)
        integer :: u, best, e, i, v, w

        dist = INF
        visited = .false.
        dist(source) = 0

        do i = 1, n
            ! Pick unvisited vertex with smallest distance
            best = -1
            do u = 1, n
                if (.not. visited(u)) then
                    if (best == -1 .or. dist(u) < dist(best)) best = u
                end if
            end do

            if (dist(best) == INF) exit
            visited(best) = .true.

            ! Relax outgoing edges
            e = adj_head(best)
            do while (e /= 0)
                v = adj_v(e)
                w = adj_w(e)
                if (dist(best) + w < dist(v)) dist(v) = dist(best) + w
                e = adj_next(e)
            end do
        end do
    end subroutine dijkstra

end module graph_module

!===========================================================
program shortest_path_compare
    use graph_module
    implicit none

    integer :: n, m, source
    integer, allocatable :: u(:), v(:), w(:)
    integer, allocatable :: dist_bf(:), dist_dij(:)

    ! adjacency list storage
    integer, allocatable :: adj_head(:), adj_next(:), adj_v(:), adj_w(:)
    logical :: has_neg_cycle

    integer :: i

    print *, "Enter number of vertices:"
    read *, n
    print *, "Enter number of edges:"
    read *, m

    allocate(u(m), v(m), w(m))
    allocate(adj_head(n), adj_next(m), adj_v(m), adj_w(m))
    allocate(dist_bf(n), dist_dij(n))

    print *, "Enter edges in format: u v weight"
    do i = 1, m
        read *, u(i), v(i), w(i)
    end do

    print *, "Enter source vertex:"
    read *, source

    !-----------------------------------------
    ! Prepare adjacency list for Dijkstra
    !-----------------------------------------
    adj_head = 0
    do i = 1, m
        adj_next(i) = adj_head(u(i))
        adj_head(u(i)) = i
        adj_v(i) = v(i)
        adj_w(i) = w(i)
    end do

    !-----------------------------------------
    ! Run Bellman-Ford
    !-----------------------------------------
    call bellman_ford(n, m, u, v, w, source, dist_bf, has_neg_cycle)

    if (has_neg_cycle) then
        print *, "Graph contains a NEGATIVE cycle. Bellman-Ford stops."
        stop
    end if

    !-----------------------------------------
    ! Run Dijkstra
    ! (assumes no negative edges)
    !-----------------------------------------
    call dijkstra(n, adj_head, adj_next, adj_v, adj_w, source, dist_dij)

    !-----------------------------------------
    ! Print comparison
    !-----------------------------------------
    print *, ""
    print *, "Shortest Path from Vertex ", source
    print *, "Vertex   Bellman-Ford    Dijkstra"
    do i = 1, n
        if (dist_bf(i) == INF) then
            write(*,'(I3,2X,A15,2X,A15)') i, "INF", "INF"
        else
            if (dist_dij(i) == INF) then
                write(*,'(I3,2X,I15,2X,A15)') i, dist_bf(i), "INF"
            else
                write(*,'(I3,2X,I15,2X,I15)') i, dist_bf(i), dist_dij(i)
            end if
        end if
    end do

end program shortest_path_compare
