
program: main.c
	@mpicc -c main.c -fopenmp
	@mpicc -o main main.o -fopenmp -lm
	@echo ">>>>> COMPILED SUCCESSFULLY <<<<<"

run:
	@mpirun --allow-run-as-root -n $(PROCESSORS) ./main

clean:
	@rm -rf *.o main
	@echo ">>>>> CLEANED SUCCESSFULLY <<<<<"

PROCESSORS ?= 1
all:
        : '$(PROCESSORS)'