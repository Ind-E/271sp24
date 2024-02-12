# include <stdio.h>

// A simple program to sort an array of integers in C

/* YOUR WORK HERE */

// Input: An array of integers and its length
// Output: Nothing
// Side Effect: The input array is sorted from least to greatest
void sort(int *arr, int len)
{
    for (int j = 0; j < len; j++) {
        int max = arr[0], maxIndex = 0;
        for (int i = 1; i < len - j; i++) {
            if (arr[i] > max) {
                max = arr[i];
                maxIndex = i;
            }
        }
        int temp = arr[maxIndex];
        arr[maxIndex] = arr[len - j - 1];
        arr[len - j - 1] = temp;
    }
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
