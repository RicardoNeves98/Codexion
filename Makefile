CFLAGS = -Wall -Wextra -Werror
 
SRCS = compile.c free_stuff.c func_coder.c func_monitor.c get_dongle.c \
        init_data.c init_threads.c main.c parsing.c queue_deadline.c \
        queue_requests.c queue_utils.c time_conversion.c time_utils.c

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
