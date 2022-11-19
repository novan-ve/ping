# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: novan-ve <novan-ve@codam.nl>                 +#+                      #
#                                                    +#+                       #
#    Created: 2021/02/01 20:11:54 by novan-ve      #+#    #+#                  #
#    Updated: 2022/03/29 14:15:42 by novan-ve      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = ft_ping

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRC = main.c \
      ping.c \
	  network.c \
      utils.c

HEADER = ping.h

OBJ := $(SRC:%.c=./$(OBJ_DIR)/%.o)
SRC := $(SRC:%=./$(SRC_DIR)/%)

HEADER := $(HEADER:%=./$(INC_DIR)/%)

FLAGS = -Wall -Werror -Wextra

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER) | $(OBJ_DIR)
	$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $^ -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	/bin/rm -f $(OBJ)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean
	$(MAKE)
