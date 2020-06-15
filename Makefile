SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.c=.h)

all: main

main: $(SOURCES) $(HEADERS)
	mpic++ $(SOURCES) -pthread -o main

clean:
	rm main

run: main
	mpirun -np 3 ./main 1 10
