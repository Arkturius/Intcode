#
# Makefile
#

NAME		:=	ic

INC_DIR		:=	include
SRCS_DIR	:=	src
OBJS_DIR	:=	.build

SRCS		:=	core.c process.c ops.c runner.c grid.c

SRCS		:=	$(addprefix $(SRCS_DIR)/, $(SRCS))
OBJS		:=	$(addprefix $(OBJS_DIR)/, $(SRCS:%.c=%.o))

CC			:=	clang
CFLAGS		:=	-Wall -Wextra -gdwarf-2 -Wno-initializer-overrides

IFLAGS		:=	-I$(INC_DIR)

RM			:=	rm -rf
DIR_DUP		:=	mkdir -p

DEBUG		?=	0

all:				$(NAME)

$(NAME):			$(OBJS) $(CPNG)
	@printf $(DEBUG_LINK) $(notdir $@)
	@$(CC) $(IFLAGS) $(CFLAGS) $^ -o $@ $(CPNG)

$(OBJS_DIR)/%.o:	%.c
	@$(DIR_DUP) $(@D)
	@printf $(DEBUG_COMP) $(notdir $@) $<
	@$(CC) $(IFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	@printf $(DEBUG_RMRF) $(notdir $(OBJS_DIR))
	@$(RM) $(OBJS_DIR)

fclean:				clean
	@printf $(DEBUG_RMRF) $(notdir $(NAME))
	@$(RM) $(NAME)

re:					fclean all

#
# Strings
#

RED			:=	"\\033[31m"
GREEN		:=	"\\033[32m"
CYAN		:=	"\\033[36m"
GREY		:=	"\\033[90m"

BOLD		:=	"\\033[1m"
ITALIC		:=	"\\033[3m"

RESET		:=	"\\033[0m"

COLOR_COMP	:=	$(BOLD)$(ITALIC)$(GREY)
COLOR_LINK	:=	$(BOLD)$(ITALIC)$(GREEN)
COLOR_RMRF	:=	$(BOLD)$(ITALIC)$(RED)

DEBUG_COMP	:=	" $(COLOR_COMP)■$(RESET) compiling $(COLOR_COMP)%-16s$(RESET) from $(COLOR_COMP)%s$(RESET)\n"
DEBUG_LINK	:=	" $(COLOR_LINK)■$(RESET) linking   $(COLOR_LINK)%-16s$(RESET)\n"
DEBUG_RMRF	:=	" $(COLOR_RMRF)■$(RESET) deleting  $(COLOR_RMRF)%-16s$(RESET)\n"
