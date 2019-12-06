CXXFLAGS= -std=c++17

all: run_first

.PHONY: run_first
run_first: first input
	./first < input

first: IntCodeComputer.h

clean:
	$(RM) first