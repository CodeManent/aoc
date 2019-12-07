CXXFLAGS= -g -std=c++17 -pthread -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -lasan -Werror=format-security -Werror=array-bounds

all: run_day7

.PHONY: run_day7
run_day7: day7 input
	./day7 < input

day7: | IntCodeComputer.h IntStream.h

clean:
	$(RM) day7