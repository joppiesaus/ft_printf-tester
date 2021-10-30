# CHANGE THE PATH OF YOUR FT_PRINTF HERE
FT_PRINTF_PATH =..

DEPS = test_printf.c
NAME = test
CFLAGS := -Wall -Wextra

all: $(NAME)

$(NAME): $(DEPS) $(FT_PRINTF_PATH)/libftprintf.a
	$(CC) $(CFLAGS) -I$(FT_PRINTF_PATH) -L$(FT_PRINTF_PATH) -lftprintf $(DEPS) -o $(NAME)

# Recursive make, sigh...
$(FT_PRINTF_PATH)/libftprintf.a: $(FT_PRINTF_PATH)/libftprintf.h $(wildcard $(FT_PRINTF_PATH)/*.c)
	$(MAKE) -C $(FT_PRINTF_PATH)

clean:
	rm -f $(NAME)

fclean: clean
	rm -f $(FT_PRINTF_PATH)/libftprintf.a

re: fclean $(NAME)
force: re

.PHONY: all clean fclean re force