CXXFLAGS= -g -std=c++17 -pthread -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -lasan -Werror=format-security -Werror=array-bounds

all: run_day9

.PHONY: run_day9
run_day9: day9 input
	./day9 < input

day9: | IntCodeComputer.h IntStream.h

clean:
	$(RM) day9
