// Maximum Subarray Sum with Divide and Conquer
// From CLRS Chapter 4

fn find_max_crossing_subarray(arr: &[i32], low: usize, mid: usize, high: usize) -> (usize, usize, i32) {
    let mut left_sum = i32::MIN;
    let mut sum = 0;
    let mut max_left = mid;

    for i in (low..=mid).rev() {
        sum += arr[i];
        if sum > left_sum {
            left_sum = sum;
            max_left = i;
        }
    }

    let mut right_sum = i32::MIN;
    sum = 0;
    let mut max_right = mid + 1;

    for j in mid + 1..=high {
        sum += arr[j];
        if sum > right_sum {
            right_sum = sum;
            max_right = j;
        }
    }

    (max_left, max_right, left_sum + right_sum)
}

fn find_max_subarray(arr: &[i32], low: usize, high: usize) -> (usize, usize, i32) {
    if low == high {
        return (low, high, arr[low]);
    }

    let mid = (low + high) / 2;
    let (left_low, left_high, left_sum) = find_max_subarray(arr, low, mid);
    let (right_low, right_high, right_sum) = find_max_subarray(arr, mid + 1, high);
    let (cross_low, cross_high, cross_sum) = find_max_crossing_subarray(arr, low, mid, high);

    if left_sum >= right_sum && left_sum >= cross_sum {
        (left_low, left_high, left_sum)
    } else if right_sum >= left_sum && right_sum >= cross_sum {
        (right_low, right_high, right_sum)
    } else {
        (cross_low, cross_high, cross_sum)
    }
}

fn main() {
    let arr = [13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7];
    let n = arr.len();
    let (low, high, sum) = find_max_subarray(&arr, 0, n - 1);

    println!("Maximum subarray found between indices {} and {}", low, high);
    println!("Maximum sum: {}", sum);
}
