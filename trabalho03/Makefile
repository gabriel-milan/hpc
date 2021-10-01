build:
	g++ -Wall -ffast-math -O3 laplace.cxx -o laplace

build_omp:
	g++ -Wall -fopenmp -ffast-math -O3 laplace_omp.cxx -o laplace_omp


clean:
	rm -f laplace
	rm -f laplace_omp
	rm -f gmon.out