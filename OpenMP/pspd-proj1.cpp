#include <omp.h>

#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include <chrono>

#define FILTER_ROW_SIZE 3
#define FILTER_COLUMN_SIZE 3

#define FILTER_MATRIX_SIZE FILTER_ROW_SIZE * FILTER_COLUMN_SIZE

// Filters reference:
// https://docs.gimp.org/2.8/en/plug-in-convmatrix.html

const int SharpenFilter[FILTER_MATRIX_SIZE] =
											{ 
												0, -1, 0,
												-1, 5,-1,
												0, -1, 0 
											};

const int BlurFilter[FILTER_MATRIX_SIZE] =
											{ 
												1, 1, 1,
												1, 1, 1,
												1, 1, 1 
											};

const int EdgeEnhanceFilter[FILTER_MATRIX_SIZE] =
											{
												0, 0, 0,
												-1, 1, 0,
												0, 0, 0
											};

const int EdgeDetectFilter[FILTER_MATRIX_SIZE] =
											{
												0, 1, 0,
												1, -4, 1,
												0, 1, 0
											};

const int EmbossFilter[FILTER_MATRIX_SIZE] =
											{
												-2, -1, 0,
												-1, 1, 1,
												0, 1, 2
											};

enum FilterType
{
	None = 0,
	Sharpen,
	Blur,
	EdgeEnhance,
	EdgeDetect,
	Emboss,
};

std::vector<int> convolution(std::vector<int> data, int rows, int columns, std::vector<int> filter)
{
	std::vector<int> output(rows * columns, 0);

	int x1, x2, a, b, x, y;

#pragma omp parallel
	#pragma omp for private(x, y, a, b, x1, x2)
		for (int z = 0; z < rows * columns; ++z)
		{
			x = z / columns;
			y = z % columns;

			for (int i = 0; i < FILTER_ROW_SIZE; ++i)
			{
				for (int j = 0; j < FILTER_COLUMN_SIZE; ++j)
				{
					a = x + i - FILTER_ROW_SIZE / 2;
					b = y + j - FILTER_COLUMN_SIZE / 2;

					x1 = (a < 0) ? rows + a : (a > rows - 1) ? a - rows : a;
					x2 = (b < 0) ? columns + b : (b > columns - 1) ? b - columns : b;

					output[x * columns + y] += data[x1 * columns + x2] * filter[i * FILTER_COLUMN_SIZE + j];

				}
			}
		}

	return output;
}

int main(int argc, char** argv)
{
	// Timer.
	std::chrono::high_resolution_clock::time_point timeNow;
	std::chrono::high_resolution_clock::duration elapsed;

	std::string inputFileName;
	inputFileName += argv[1];

	// Get heigth.
	int height = atoi(argv[2]);

	// Get width.
	int width = atoi(argv[3]);

	// Set type of filter.
	int filter = atoi(argv[4]);

	// Number of threads.
	int threads = atoi(argv[5]);
	if (threads <= 0)
		threads = omp_get_max_threads();

	omp_set_num_threads(threads);

	std::vector<int> filtervec(FILTER_ROW_SIZE * FILTER_COLUMN_SIZE);

	switch (filter)
	{
		case Sharpen:
		{
			for (int i = 0; i < FILTER_MATRIX_SIZE; i++)
			{
				filtervec[i] = SharpenFilter[i];
			}
		}
		break;
		case Blur:
		{
			for (int i = 0; i < FILTER_MATRIX_SIZE; i++)
			{
				filtervec[i] = BlurFilter[i];
			}
		}
		break;
		case EdgeEnhance:
		{
			for (int i = 0; i < FILTER_MATRIX_SIZE; i++)
			{
				filtervec[i] = EdgeEnhanceFilter[i];
			}
		}
		break;
		case EdgeDetect:
		{
			for (int i = 0; i < FILTER_MATRIX_SIZE; i++)
			{
				filtervec[i] = EdgeDetectFilter[i];
			}
		}
		break;
		case Emboss:
		{
			for (int i = 0; i < FILTER_MATRIX_SIZE; i++)
			{
				filtervec[i] = EmbossFilter[i];
			}
		}
		break;
		default:
		{
			std::cerr << "Invalid filter!!" << std::endl;
			return -1;
		}
	}

	std::ifstream inputData(inputFileName, std::ios::in);
	if (!inputData.is_open())
	{
		std::cerr << "File not loaded, maybe not found!!" << std::endl;
		return -2;
	}

	std::vector<int> matrixData(height * width);

	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			inputData >> matrixData[i * width + k];
		}
	}

	timeNow = std::chrono::high_resolution_clock::now();
	std::vector<int> out = convolution(matrixData, height, width, filtervec);

	std::ofstream outData("./input/output.txt", std::ios::out);

	for (int i = 0; i < height; i++) 
	{
		for (int j = 0; j < width; j++) 
{
			if (j != width - 1) outData << out[i * width + j] << ' ';
			else outData << out[i * width + j];
		}

		outData << std::endl;
	}

	elapsed = std::chrono::high_resolution_clock::now() - timeNow;

	std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() << " nanosecs" << std::endl;

	inputData.close();
	outData.close();

	return 0;
}
