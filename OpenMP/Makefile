program: pspd-proj1.cpp
	@g++ -fopenmp pspd-proj1.cpp --std=c++11 -g -Wall
	@echo ">>>>> COMPILED SUCCESSFULLY <<<<<"

run:
	@./a.out "$(INPUT_PATH)" $(ROW_SIZE) $(COLUMN_SIZE) $(FILTER_TYPE) $(THREADS)

clean:
	@rm -rf *.o a.out ./input/output.txt
	@echo ">>>>> CLEANED SUCCESSFULLY <<<<<"

INPUT_PATH ?= ./input/input2.txt
ROW_SIZE ?= 1000
COLUMN_SIZE ?= 10000
FILTER_TYPE ?= 3
THREADS ?= 0
all:
        : '$(INPUT_PATH)'
        : '$(ROW_SIZE)'
        : '$(COLUMN_SIZE)'
        : '$(FILTER_TYPE)'
        : '$(THREADS)'