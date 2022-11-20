ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

SRC_DIR = ./src/

HEADER = malloc.h

HEADER := $(addprefix ./include/, $(HEADER))

INCL_FOLDERS = $(dir $(HEADER))

INCL := $(addprefix -I ,$(INCL_FOLDERS))

SRC = free.c \
      malloc.c \
	  realloc.c \
	  show_mem.c \
	  show_mem_ex.c \
	  utils.c 

OBJ := $(SRC:%.c=./obj/%.o)

FLAGS = -Wall -Werror -Wextra

all: $(NAME)

obj/%.o: src/%.c $(HEADER)
	mkdir -p $(@D)
	$(CC) -fPIC $(FLAGS) $(INCL) -c $< -o $@

$(NAME): $(OBJ) $(HEADER)
	$(CC) $(OBJ) -o $@ -shared
	ln -sf $(NAME) $(LINK)

clean:
	/bin/rm -f $(OBJ) $(BOBJ)

fclean: clean
	/bin/rm -f $(NAME) $(LINK)

re: fclean
	$(MAKE)
