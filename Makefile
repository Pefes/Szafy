SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.c=.h)

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) -pthread -o main -lstdc++

clean:
	rm main

run: main
	mpirun -np 3 ./main 2 10
