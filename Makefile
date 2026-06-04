.PHONY: run

bin/knight: src/knight.c
		@mkdir -p bin
		@gcc -Iinclude -o bin/knight -Wall -Werror -Wextra -pedantic src/knight.c

run: bin/knight
	@./bin/knight $(args)