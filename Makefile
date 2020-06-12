SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) -o main

clean:
	rm main

run: main
	mpirun -np 3 ./main 2 5
