#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>

// rip from https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C
// try "convert x.ppm x.png" and follow the install instructions to get a png

// In C, we make a 3 dimension unsigned char array of size * size * 3
// Hint - how many times should loop? How many times should you call malloc?
unsigned char ***create_base(int size)
{
	unsigned char ***arr = malloc(size * sizeof(unsigned char **));
	for (int i = 0; i < size; i++)
	{
		unsigned char **cur_row = malloc(size * sizeof(unsigned char *));
		for (int j = 0; j < size; j++)
		{
			cur_row[j] = malloc(3 * sizeof(unsigned char));
			for (int k = 0; k < 3; k++)
			{
				cur_row[j][k] = 0;
			}
		}
		arr[i] = cur_row;
	}
	return arr;
}

// Calculate z_(n+1) = z_n^2 + c and return the result
// in C, we accept two complex values and produce a complex output
// I've included sample code to work with complex values.
// Hint - don't use exponentiation
double complex m_seq(double complex z_n, double complex c)
{
	return z_n * z_n + c;
}

// in C we accept a complex value and an integer size and two integer pointers, and populate the integer points with the x and y results
// I've included sample code to zero out x and y.
void c2b(double complex c, int size, int *x, int *y)
{
	*x = (int)((creal(c) + 2.0) * ((double)size / 4.0));
	*y = (int)((cimag(c) + 2.0) * ((double)size / 4.0));

	*x = (*x > size - 1) ? size - 1 : *x;
	*y = (*y > size - 1) ? size - 1 : *y;
	*x = (*x < 0) ? 0 : *x;
	*y = (*y < 0) ? 0 : *y;
	return;
}

// in C, we use b2c to loop over all pixels rather than relying on randomization
// return the complex associated with a location x, y
// I've included sample code to work with complex values.
double complex b2c(int size, int x, int y)
{
	double a = x * 4.0 / size - 2.0, b = y * 4.0 / size - 2.0;
	double complex r = a + b * I;
	return r;
}

// in C we accept a complex value, and integer number of iterations, and returns with an int that represents whether c escapes, or exceeds absolute value 2 in iters applications of m_seq.
// I included the absolute value sample code
int escapes(double complex c, int iters)
{
	double complex z_n = c;
	for (int i = 0; i < iters; i++)
	{
		z_n = m_seq(z_n, c);
		if (abs(z_n) > 2)
			return 1;
	}
	return 0;
}

// in C, we accept a 3d array base, an integer for size and for iterations, a color channel of 0,1,2, and a complex value c
void one_val(unsigned char ***base, int size, int iters, int color, double complex c)
{
	if (escapes(c, iters) == 0)
	{
		return;
	}
	double complex z_n = c;
	for (int i = 0; i < iters; i++)
	{
		if (abs(z_n) > 2)
			return;
		int x = 0, y = 0;
		c2b(z_n, size, &x, &y);
		x = (x > size - 1) ? size - 1 : x;
		y = (y > size - 1) ? size - 1 : y;
		int v = base[x][y][color];
		v += 20;
		if (v > 255)
			v = 255;
		base[x][y][color] = v;
		z_n = m_seq(z_n, c);
	}
	return;
}

// in C, we accept a 3d array base, an integer for size and for iterations
void get_colors(unsigned char ***base, int size, int iters)
{
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int i = 0; i < 3; i++)
			{
				one_val(base, size, iters * pow(10, i), i, b2c(size, x, y));
			}
		}
	}
	return;
}

// OPTIONAL
// That said, you images will look bad without this.
// The Python sample had a hacky solution.
// We accept a base, and equalize values to percentiles rather than counts
// You equalized images in CS 151 ImageShop.
void equalize(unsigned char ***base, int size)
{
	int hist[256] = {0};
	int remap[256] = {0};
	int total_pixels = size * size;

	// Calculate histogram
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				hist[base[i][j][k]]++;
			}
		}
	}

	// Calculate cumulative distribution
	for (int i = 1; i < 256; i++)
	{
		hist[i] += hist[i - 1];
	}

	// Calculate remapping values
	for (int i = 0; i < 256; i++)
	{
		remap[i] = (hist[i] * 255) / total_pixels;
	}

	// Apply remapping to image
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				base[i][j][k] = remap[base[i][j][k]];
			}
		}
	}
}

void contrast_stretching(unsigned char ***base, int size)
{
	unsigned char min = 255, max = 0;

	// Find the min and max pixel values
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (base[i][j][k] < min)
					min = base[i][j][k];
				if (base[i][j][k] > max)
					max = base[i][j][k];
			}
		}
	}

	// Apply contrast stretching
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				base[i][j][k] = (unsigned char)(((base[i][j][k] - min) * 255) / (max - min));
			}
		}
	}
}
// Sigmoid function
double sigmoid(double x)
{
	return 1 / (1 + exp(-1024 * x));
}

// Sigmoid scaling
void sigmoid_scale(unsigned char ***base, int size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				// Normalize pixel values to range [-1, 1]
				double normalized = (double)base[i][j][k] / 127.5 - 1;

				// Apply sigmoid function and scale back to [0, 255]
				base[i][j][k] = (unsigned char)((sigmoid(normalized) + 1) * 127.5);
			}
		}
	}
}

void darker(unsigned char ***base, int size)
{
	unsigned char ***temp = create_base(size);

	for (int i = 1; i < size - 1; i++)
	{
		for (int j = 1; j < size - 1; j++)
		{
			int black_neighbors = 0;

			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					if (base[i + x][j + y][0] == 0 && base[i + x][j + y][1] == 0 && base[i + x][j + y][2] == 0)
					{
						black_neighbors++;
					}
				}
			}

			if (black_neighbors >= 6)
			{
				temp[i][j][0] = 0;
				temp[i][j][1] = 0;
				temp[i][j][2] = 0;
			}
			else
			{
				temp[i][j][0] = base[i][j][0];
				temp[i][j][1] = base[i][j][1];
				temp[i][j][2] = base[i][j][2];
			}
		}
	}

	// Copy the modified pixels back to the original base
	for (int i = 1; i < size - 1; i++)
	{
		for (int j = 1; j < size - 1; j++)
		{
			base[i][j][0] = temp[i][j][0];
			base[i][j][1] = temp[i][j][1];
			base[i][j][2] = temp[i][j][2];
		}
	}
}

// Given an edge size and starting iteration count, make a buddhabrot.
// I'm leaving the ppm code
void make_brot(int size, int iters)
{
	FILE *fp = fopen("brot.ppm", "wb"); /* b - binary mode */
	fprintf(fp, "P6\n%d %d\n255\n", size, size);

	fflush(stdout);

	unsigned char ***base = create_base(size);
	get_colors(base, size, iters);
	// sigmoid_scale(base, size);
	// contrast_stretching(base, size);
	// darker(base, size);

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			fwrite(base[x][y], 1, 3, fp);
		}
	}
	fclose(fp);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			free(base[i][j]);
		}
		free(base[i]);
	}
	free(base);

	return;
}

int main()
{
	make_brot(600, 10);
	return 0;
}
