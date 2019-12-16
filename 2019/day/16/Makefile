CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -Werror=format-security -Werror=array-bounds

all: run_day16

.PHONY: run_day16
run_day16: day16 input
	./day16 < input

clean:
	$(RM) day16
