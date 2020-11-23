NAME=haz
FLAGS=-Wall -Wextra -Wpedantic -g -ggdb

$(NAME): main.c
	$(CC) -o $(NAME) $(FLAGS) $<
