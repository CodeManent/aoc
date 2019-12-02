CXXFLAGS= -std=c++17

all: run_first run_second

.PHONY: run_first run_second
run_first: first input/input
	./first < input/input

run_second: second input/input
	./second < input/input

clean:
	$(RM) first second