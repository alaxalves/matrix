##Args:

"C:\example\input\input1.txt" 128  512 3 0

1. path from the input.
2. height.
3. width.
4. type of filter {	Sharpen, Blur = 2, EdgeEnhance = 3, EdgeDetect = 4, Emboss = 5 }
5. number of threads. (0 = omp_get_max_threads)

## Linux

1. Install openmp:
sudo apt-get install libomp-dev

2. Compile:
g++ -fopenmp main.cpp --std=c++11 -g -Wall

3. Execute:
./a.out "/home/litwin/trab1/input/input2.txt" 1000 10000 3 0
