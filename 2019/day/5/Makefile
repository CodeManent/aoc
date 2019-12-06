CXXFLAGS= -std=c++17

all: run_first run_second

.PHONY: run_first run_second
run_first: first input
	./first < input

run_second: second input
	./second < input

first: IntCodeComputer.h
second: IntCodeComputer.h

clean:
	$(RM) first second