NAME		=	ft_nm
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror -I $(INCLUDE)
SRCDIR		=	./src/
OBJDIR		=	./obj/
INCLUDE		=	./include/

SRC			=	main.c	\
					
OBJ			=	${addprefix $(OBJDIR), $(SRC:%.c=%.o)}


# ===== #


all:			$(NAME)

$(NAME):		$(OBJDIR) $(OBJS)
				#@make -C libft/
				$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
				rm -rf $(OBJDIR)
				#@make -C libft/ clean

fclean:			
				rm -rf $(NAME) $(OBJDIR)
				#@make -C libft/ fclean

re:				fclean all

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean objs all