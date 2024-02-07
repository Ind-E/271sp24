# include <stdio.h>

// A simple program to sort an array of integers in C

/* YOUR WORK HERE */

// Input: An array of integers, its length, and the digit to sort
// Output: Nothing
// Side Effect: The input array is sorted from least to greatest based on that digit
void countSort(int arr[], int len, int exp)
{
    // Output array
    int output[len];
    int i, count[10] = { 0 };
 
    // Store count of occurrences
    // in count[]
    for (i = 0; i < len; i++)
        count[(arr[i] / exp) % 10]++;
 
    // Change count[i] so that count[i]
    // now contains actual position
    // of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];
 
    // Build the output array
    for (i = len - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }
 
    // Copy the output array to arr[], so that arr[] now contains sorted numbers according to current digit
    for (i = 0; i < len; i++)
        arr[i] = output[i];
}

// Input: An array of integers and its length
// Output: Nothing
// Side Effect: The input array is sorted from least to greatest
void sort(int *arr, int len) {
	int max = arr[0];
	for (int i = 0; i < len; i++) {
		if (arr[i] > max) // if (((max + (~arr[i] + 1)) >> 31) & 1)
			max = arr[i];
	}
 
    // Do counting sort for every digit.
    // Note that instead of passing digit
    // number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; max / exp > 0; exp *= 10)
        countSort(arr, len, exp);
}

/* END YOUR WORK */

/* FEEDBACK FUNCTIONS */

// Input: An array of integers and its length
// Output: 1 if the array is sorted least to greatest, 0 otherwise
// Side Effect: None
int is_sorted(int *arr, int len)
{
	int i = 0;
	while (i < (len - 2))
	{
		if (arr[i] > arr[i+1])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

// The special main function

int main()
{
	int test_arr[10] = {8,6,4,2,0,1,3,5,7,9};
	sort(test_arr, 10);
	if (is_sorted(test_arr, 10))
	{
		printf("Good job!\n");
	} else {
		printf("Keep it up!\n");
	}
	return 0;
}
