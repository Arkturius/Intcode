/**
* core.c
*/

#include <assert.h>
#include <intcode.h>

ICC_Core	core = {0};

void
icc(core_spawn_proc, ICC(Core *) core, char *code)
{
	ICC(Proc)	proc = {0};

	icc(proc_start, &proc, 0);
	icc(proc_load, &proc, code);

	stk_push(&core->procs, proc);
}

bool
icc(read_str, char **line)
{
	u64		size;
	int		ret = getline(line, &size, stdin);

	return (ret != -1);
}

bool
icc(read_int, u32 *n)
{
	char	*line = NULL;

	if (!icc(read_str, &line))
	{
		free(line);
		return (false);
	}
	sscanf(line, "%1u", n);
	free(line);
	return (true);
}

u32
icc(menu)
{
	char	*line = NULL;

	printf("[ ] > ");
	if (icc(read_str, &line) == -1)
		icc(panic, "getline failed.");
	
	Strings	args = {0};

	do
	{
		char	*tk = strtok(line, " \t\n\v\f\r");

		if (!tk)
			break ;
		stk_push(&args, strdup(tk));
		line = NULL;
	}
	while (1);

	stk_foreach(arg, &args)
	{
		
	}

	char	*commands[] = 
	{
		"new", "run", "del",
		"link", "show", "exit",
		NULL,
	};

	u32 choice = 0;	
	return (choice);
}

static void
icc(menu_proc_load)
{
	char	*code = NULL;

	if (icc(read_str, &code))
		icc(core_spawn_proc, &core, code);
	else
		core.running = false;
	free(code);
}

static void
icc(menu_proc_dup)
{
}

static void
icc(menu_proc_run)
{
	u32	id = 0;

	if (!icc(read_int, &id) || id >= stk_size(&core.procs))
	{
		icc(log, "invalid process ID.");
		return ;
	}
	ICC(Proc *)	proc = stk_data(&core.procs) + id;

	icc(proc_run, proc);
}

static void
icc(menu_proc_watch)
{

}

static void
icc(menu_exit)
{
	icc(log, "exiting...");
	core.running = false;
}

static void
icc(menu_proc_link)
{
	u32	id = 0;

	if (!icc(read_int, &id))
	{
		icc(log, "invalid process ID.");
		return ;
	}
	ICC(Proc *)	proc1 = stk_data(&core.procs) + id;
	
	if (!icc(read_int, &id))
	{
		icc(log, "invalid process ID.");
		return ;
	}
	ICC(Proc *)	proc2 = stk_data(&core.procs) + id;
	icc(proc_link, proc1, proc2, ICC_LINK_OUT);
}

typedef void	(*icc_menu_fn)(void);

static	icc_menu_fn	menu[] = 
{
	ICC(menu_proc_load),
	ICC(menu_proc_dup),
	ICC(menu_proc_run),
	ICC(menu_proc_watch),
	ICC(menu_proc_link),
	ICC(menu_exit),
};


void
icc(core_init, ICC(Core *) core)
{
	if (!core)
		icc(panic, "%s: invalid core pointer. (0x%016lx)", __func__, (uptr)core);

	core->running = true;
	core->memory = (Words){0}; // TODO: make proc pointers point into this memory location
	// offset each proc by a good amount to avoid the programs writing to another proc memory space
	core->procs = (ICC(Procs)){0};
}

void
icc(core_destroy, ICC(Core *) core)
{
	if (!core)
		icc(panic, "%s: invalid core pointer. (0x%016lx)", __func__, (uptr)core);

	core->running = false;
	for (u32 i = 0; i < core->procs.size; ++i)
		icc(proc_stop, stk_data(&core->procs) + i);

	free(stk_data(&core->memory));
	free(stk_data(&core->procs));
}

void
icc(core_routine, ICC(Core *) core)
{
	while (core->running)
	{
		u32		choice = ~0;
		
		choice = icc(menu);
		if (!core->running)
			break ;
		if (choice >= array_size(menu))
		{
			icc(err, "invalid choice '%u'", choice);
			continue ;
		}
		menu[choice]();
	}
}

#include <termios.h>

void
icc(core_gui, ICC(Core *) core, bool restore)
{
	static struct termios	orig = {0};
	struct termios			curr = {0};

	if (!isatty(STDIN_FILENO))
		return ;
	tcgetattr(STDIN_FILENO, &curr);
	if (!restore)
	{
		orig = curr;
		curr.c_lflag &= ~(ICANON | ECHO);
		curr.c_cc[VMIN] = 0;
		curr.c_cc[VTIME] = 0;
		printf("\033[?25l");
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &curr);
	}
	else
	{
		printf("\033[?25h");
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	}
	core->gui = true;
}

int
main(void)
{
	ICC(Core)	core;

	icc(core_init, &core);
	icc(core_gui, &core, false);
	icc(core_routine, &core);
	icc(core_gui, &core, true);
	icc(core_destroy, &core);
	return (0);
}
