CXXFLAGS= -std=c++17

all: run_first run_second

.PHONY: run_first run_second
run_first: first
	./first

run_second: second
	./second

clean:
	$(RM) first second