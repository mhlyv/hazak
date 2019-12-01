NAME=haz
FLAGS=-Wall -Wextra -pedantic

$(NAME): main.c
	$(CC) -o $(NAME) $(FLAGS) $<
