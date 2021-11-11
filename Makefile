# CHANGE THE PATH OF YOUR FT_PRINTF HERE
FT_PRINTF_PATH =..

DEPS = test_printf.c
NAME = test
# does your printf crash? Add -fsanitize=address here to see more info!
CFLAGS := -Wall -Wextra -Wno-format

all: $(NAME)

$(NAME): $(DEPS) $(FT_PRINTF_PATH)/libftprintf.a
	$(CC) $(CFLAGS) -I$(FT_PRINTF_PATH) -L$(FT_PRINTF_PATH) -lftprintf $(DEPS) -o $(NAME)

# Recursive make, sigh...
$(FT_PRINTF_PATH)/libftprintf.a: $(wildcard $(FT_PRINTF_PATH)/*.h) $(wildcard $(FT_PRINTF_PATH)/*.c)
	$(MAKE) -C $(FT_PRINTF_PATH)

clean:
	rm -f $(NAME)

fclean: clean
	rm -f $(FT_PRINTF_PATH)/libftprintf.a

re: fclean $(NAME)
force: re

.PHONY: all clean fclean re force
