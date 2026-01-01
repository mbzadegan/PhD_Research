!===========================================================
! Johnson's Algorithm for All-Pairs Shortest Paths
! Fortran 90+
!===========================================================

module graph_module
    implicit none
    integer, parameter :: INF = 2147483647
contains

    !-------------------------------------------------------
    subroutine bellman_ford(n, m, u, v, w, h, has_negative_cycle)
        implicit none
        integer, intent(in) :: n, m
        integer, intent(in) :: u(:), v(:)
        integer, intent(in) :: w(:)
        integer, intent(out) :: h(:)
        logical, intent(out) :: has_negative_cycle
        integer :: i, j

        h = 0  ! initialize potentials

        ! Relax edges n-1 times
        do i = 1, n-1
            do j = 1, m
                if (h(u(j)) /= INF .and. h(u(j)) + w(j) < h(v(j))) then
                    h(v(j)) = h(u(j)) + w(j)
                end if
            end do
        end do

        ! Detect negative cycles
        has_negative_cycle = .false.
        do j = 1, m
            if (h(u(j)) /= INF .and. h(u(j)) + w(j) < h(v(j))) then
                has_negative_cycle = .true.
                return
            end if
        end do
    end subroutine bellman_ford

    !-------------------------------------------------------
    subroutine dijkstra(n, adj_head, adj_next, adj_v, adj_w, source, dist)
        implicit none
        integer, intent(in) :: n, source
        integer, intent(in) :: adj_head(:), adj_next(:), adj_v(:), adj_w(:)
        integer, intent(out) :: dist(:)

        logical :: visited(n)
        integer :: i, u, v, w, best, e

        dist = INF
        visited = .false.
        dist(source) = 0

        do i = 1, n
            ! Choose nearest unvisited node
            best = -1
            do u = 1, n
                if (.not. visited(u)) then
                    if (best == -1 .or. dist(u) < dist(best)) best = u
                end if
            end do

            if (dist(best) == INF) exit
            visited(best) = .true.

            ! Relax neighbors
            e = adj_head(best)
            do while (e /= 0)
                v = adj_v(e)
                w = adj_w(e)
                if (dist(best) + w < dist(v)) then
                    dist(v) = dist(best) + w
                end if
                e = adj_next(e)
            end do
        end do
    end subroutine dijkstra

end module graph_module

!===========================================================
program johnson_algorithm
    use graph_module
    implicit none

    integer :: n, m
    integer, allocatable :: u(:), v(:), w(:)
    integer, allocatable :: h(:)
    logical :: bad_cycle

    ! adjacency structure for reweighted graph
    integer, allocatable :: adj_head(:), adj_next(:), adj_v(:), adj_w(:)
    integer :: i, e

    integer, allocatable :: dist(:), all_dist(:,:)

    print *, "Enter number of vertices (n):"
    read *, n
    print *, "Enter number of edges (m):"
    read *, m

    allocate(u(m), v(m), w(m))
    allocate(h(n))
    allocate(adj_head(n), adj_next(m), adj_v(m), adj_w(m))
    allocate(dist(n))
    allocate(all_dist(n,n))

    print *, "Enter edges: u v weight"
    do i = 1, m
        read *, u(i), v(i), w(i)
    end do

    !---------------------------------------
    ! Step 1: Run Bellman–Ford from dummy node
    !---------------------------------------
    call bellman_ford(n, m, u, v, w, h, bad_cycle)

    if (bad_cycle) then
        print *, "Graph contains a NEGATIVE cycle. Johnson's algorithm cannot continue."
        stop
    end if

    !---------------------------------------
    ! Step 2: Reweight edges w' = w + h[u] - h[v]
    !---------------------------------------
    adj_head = 0
    do i = 1, m
        adj_next(i) = adj_head(u(i))
        adj_head(u(i)) = i
        adj_v(i) = v(i)
        adj_w(i) = w(i) + h(u(i)) - h(v(i))
    end do

    !---------------------------------------
    ! Step 3: Run Dijkstra for each vertex
    !---------------------------------------
    do i = 1, n
        call dijkstra(n, adj_head, adj_next, adj_v, adj_w, i, dist)

        ! Recover original distances
        do e = 1, n
            if (dist(e) < INF) then
                all_dist(i,e) = dist(e) - h(i) + h(e)
            else
                all_dist(i,e) = INF
            end if
        end do
    end do

    !---------------------------------------
    ! Print result matrix
    !---------------------------------------
    print *, ""
    print *, "All-Pairs Shortest Path Distance Matrix:"
    do i = 1, n
        do e = 1, n
            if (all_dist(i,e) == INF) then
                write(*,'(A8)', advance='no') "INF"
            else
                write(*,'(I8)', advance='no') all_dist(i,e)
            end if
        end do
        print *, ""
    end do

end program johnson_algorithm
