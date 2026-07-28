CFLAGS = -Wall -Wextra -Werror

SRCS = main.c parsing.c init_threads.c
OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	cc $(CFLAGS) -pthread -o program $(OBJS)

debug: $(OBJS)
	gcc -g $(CFLAGS) -pthread -o program $(OBJS)

%.o: %.c
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f program

re: fclean all

.PHONY: all debug clean fclean re
