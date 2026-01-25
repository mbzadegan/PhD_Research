! ------------------------------------------------------------------
! Burrows-Wheeler Transform (BWT) Implementation
! Based on concepts in the "Introduction to Data Compression" book by K. Sayood
!
! This module demonstrates:
! 1. Permutation of text via Block Sorting (Rotations).
! 2. The "primary index" problem (tracking the original row).
! 3. The Inverse BWT using the "LF Mapping" property.
! ------------------------------------------------------------------

module bwt_mod
    implicit none
    private
    public :: bwt_encode, bwt_decode

contains

    ! --------------------------------------------------------------
    ! BWT Encoder
    ! Input:  Original string
    ! Output: Transformed string (L) and the Primary Index (I)
    ! --------------------------------------------------------------
    subroutine bwt_encode(input_str, output_str, primary_idx)
        character(len=*), intent(in)  :: input_str
        character(len=*), intent(out) :: output_str
        integer, intent(out)          :: primary_idx

        integer :: n, i
        integer, allocatable :: indices(:)

        n = len(input_str)
        allocate(indices(n))

        ! Initialize indices [1, 2, ..., N]
        do i = 1, n
            indices(i) = i
        end do

        ! Sort indices based on the rotation of input_str they represent
        ! (Using a simple Shell Sort for demonstration)
        call sort_rotations(indices, input_str, n)

        ! Construct the Last Column (L)
        ! The last char of a rotation starting at i is at index (i-2) mod n
        do i = 1, n
            output_str(i:i) = get_char_at_rotation_end(indices(i), input_str, n)
            
            ! Identify the row that corresponds to the original string (rotation 1)
            if (indices(i) == 1) primary_idx = i
        end do

        deallocate(indices)
    end subroutine bwt_encode

    ! --------------------------------------------------------------
    ! BWT Decoder (The "Hard" Part)
    ! Uses the LF Mapping Property:
    ! The k-th occurrence of character 'c' in the Last Column (L)
    ! corresponds to the k-th occurrence of 'c' in the First Column (F).
    ! --------------------------------------------------------------
    subroutine bwt_decode(bwt_str, primary_idx, recovered_str)
        character(len=*), intent(in)  :: bwt_str
        integer, intent(in)           :: primary_idx
        character(len=*), intent(out) :: recovered_str

        integer :: n, i, j, curr_char_code
        integer :: count(0:255)       ! Frequency of each char
        integer :: start_pos(0:255)   ! Starting index of each char in F column
        integer, allocatable :: T(:)  ! Transformation Vector (LF Mapping)
        character(len=1) :: c

        n = len(bwt_str)
        allocate(T(n))

        ! 1. Calculate Frequencies
        count = 0
        do i = 1, n
            curr_char_code = iachar(bwt_str(i:i))
            count(curr_char_code) = count(curr_char_code) + 1
        end do

        ! 2. Calculate Starting Positions for 'F' column (First Column)
        ! This simulates sorting the BWT string without actually sorting it.
        j = 1
        do i = 0, 255
            start_pos(i) = j
            j = j + count(i)
            count(i) = 0 ! Reset count to use as current counter in next step
        end do

        ! 3. Build the Transformation Vector T (LF Mapping)
        ! T[i] maps the character at L[i] to its position in F.
        do i = 1, n
            curr_char_code = iachar(bwt_str(i:i))
            ! The position in F is the start_pos of this char + how many we've seen so far
            T(i) = start_pos(curr_char_code) + count(curr_char_code)
            count(curr_char_code) = count(curr_char_code) + 1
        end do

        ! 4. Reconstruct the String
        ! We traverse T backwards (or forwards depending on standard) to rebuild S.
        ! Algorithm: 
        ! j = primary_index
        ! for i = N to 1:
        !    S[i] = L[j]  (or F[j], but L[T[j]] logic is standard reverse traversal)
        !    j = T[j]
        
        j = primary_idx
        do i = n, 1, -1
            ! The character in the Last column at the current row
            recovered_str(i:i) = bwt_str(j:j)
            
            ! Jump to the row in F where this character starts
            j = T(j)
        end do

        deallocate(T)
    end subroutine bwt_decode

    ! --------------------------------------------------------------
    ! Helper: Get the last character of a rotation starting at 'start_idx'
    ! --------------------------------------------------------------
    function get_char_at_rotation_end(start_idx, str, n) result(c)
        integer, intent(in) :: start_idx, n
        character(len=*), intent(in) :: str
        character(len=1) :: c
        integer :: end_idx

        ! If rotation starts at 1, it ends at N.
        ! If rotation starts at 2, it ends at 1...
        end_idx = start_idx - 1
        if (end_idx == 0) end_idx = n
        
        c = str(end_idx:end_idx)
    end function get_char_at_rotation_end

    ! --------------------------------------------------------------
    ! Helper: Compare two rotations of the string
    ! Returns .true. if Rotation(idx1) < Rotation(idx2)
    ! --------------------------------------------------------------
    function is_rotation_less(idx1, idx2, str, n) result(res)
        integer, intent(in) :: idx1, idx2, n
        character(len=*), intent(in) :: str
        logical :: res
        integer :: i, k1, k2
        
        res = .false.
        if (idx1 == idx2) return

        do i = 0, n-1
            k1 = mod(idx1 + i - 1, n) + 1
            k2 = mod(idx2 + i - 1, n) + 1
            
            if (str(k1:k1) < str(k2:k2)) then
                res = .true.
                return
            else if (str(k1:k1) > str(k2:k2)) then
                res = .false.
                return
            end if
        end do
    end function is_rotation_less

    ! --------------------------------------------------------------
    ! Helper: Sort Indices based on Rotations (Shell Sort)
    ! --------------------------------------------------------------
    subroutine sort_rotations(indices, str, n)
        integer, intent(inout) :: indices(:)
        character(len=*), intent(in) :: str
        integer, intent(in) :: n
        integer :: gap, i, j, temp

        gap = n / 2
        do while (gap > 0)
            do i = gap + 1, n
                temp = indices(i)
                j = i
                do while (j > gap)
                    if (is_rotation_less(temp, indices(j - gap), str, n)) then
                        indices(j) = indices(j - gap)
                        j = j - gap
                    else
                        exit
                    end if
                end do
                indices(j) = temp
            end do
            gap = gap / 2
        end do
    end subroutine sort_rotations

end module bwt_mod

! ------------------------------------------------------------------
! Main Program Driver
! ------------------------------------------------------------------
program bwt_test
    use bwt_mod
    implicit none

    character(len=20) :: original = "BANANAS"
    character(len=20) :: encoded, recovered
    integer :: pid, n

    n = len_trim(original)
    
    ! Adjust string length for clean output
    write(*,*) "Original:  ", original(1:n)

    ! 1. Encode
    call bwt_encode(original(1:n), encoded, pid)
    write(*,*) "Encoded:   ", encoded(1:n)
    write(*,*) "Index:     ", pid

    ! 2. Decode
    call bwt_decode(encoded(1:n), pid, recovered)
    write(*,*) "Recovered: ", recovered(1:n)

    ! Verification
    if (original(1:n) == recovered(1:n)) then
        write(*,*) "SUCCESS: Strings match."
    else
        write(*,*) "FAILURE: Strings do not match."
    end if

end program bwt_test
