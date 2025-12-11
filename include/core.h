/**
* core.h
*/

#if !defined (_ICC_CORE)
# define _ICC_CORE

# include <common.h>
# include <process.h>

typedef struct	icc_core_s
{
	ICC(Procs)	procs;
	Words		memory;

	bool	running;
	bool	gui;
}
ICC(Core);

void
icc(core_init, ICC(Core *));

void
icc(core_spawn_proc, ICC(Core *), char *code);

#endif // _ICC_CORE
