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

void
icc(menu)
{
	icc(log, "==== INTCODE ====");
	icc(log, "(0) Load process");
	icc(log, "(1) Duplicate process");
	icc(log, "(2) Run process");
	icc(log, "(3) Watch process");
	icc(log, "(4) Exit");
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

void
icc(menu_choice, u32 *choice)
{
	dprintf(2, "[+] >>> ");
	if (!icc(read_int, choice))
		core.running = false;
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

	if (!icc(read_int, &id))
	{
		icc(log, "invalid process ID.");
		return ;
	}
	ICC(Proc *)	proc = stk_data(&core.procs) + id;

	icc(runner_start, proc);
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

typedef void	(*icc_menu_fn)(void);

static	icc_menu_fn	menu[] = 
{
	ICC(menu_proc_load),
	ICC(menu_proc_dup),
	ICC(menu_proc_run),
	ICC(menu_proc_watch),
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

int
main(void)
{
	icc(core_init, &core);
	while (core.running)
	{
		u32	choice = ~0;

		icc(menu);
		icc(menu_choice, &choice);
		if (!core.running)
			break ;
		if (choice > array_size(menu))
		{
			icc(err, "invalid choice '%u'", choice);
			continue ;
		}
		menu[choice]();
	}
	icc(core_destroy, &core);
	return (0);
}
