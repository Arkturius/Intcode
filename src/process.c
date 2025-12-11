/**
* process.c
*/

#include <intcode.h>
#include <runner.h>

#define	PROC_MEM_CAP	0x80000

static void
icc(proc_mem_acquire, ICC(Proc *) p)
{
	if (!p)
		icc(panic, " %s: no process specified.", __func__);

	if (!stk_data(&p->mem))
	{
		stk_data(&p->mem) = calloc(PROC_MEM_CAP, sizeof(Word));
		stk_size(&p->mem) = 0;
		stk_cap(&p->mem) = PROC_MEM_CAP;
	}
}

static void
icc(proc_mem_release, ICC(Proc *) p)
{
	if (!p)
		icc(panic, " %s: no process specified.", __func__);

	free(stk_data(&p->mem));
	stk_size(&p->mem) = 0;
	stk_cap(&p->mem) = 0;
}

void
icc(proc_start, ICC(Proc *) p, Word pc)
{
	icc(proc_mem_acquire, p);
	p->pc = pc;
	p->fd_in = 0;
	p->fd_out = 1;
}

void
icc(proc_stop, ICC(Proc *) p)
{
	icc(proc_mem_release, p);
	p->pc = ~0ull;
	p->fd_in = -1;
	p->fd_out = -1;
}

void
icc(proc_write, ICC(Proc *) p, Addr pos, Word n)
{
	Word	*mem = stk_data(&p->mem);

	if (pos >= stk_cap(&p->mem))
		icc(panic, "%s: OOB write [%ld].", __func__, pos);
	mem[pos] = n;

	u8	bit_idx = pos & 63;
	u32	idx = pos >> 6;
	p->changed[idx] |= (1ull << bit_idx);
}

Word
icc(proc_read, ICC(Proc *) p, Addr pos)
{
	Word	*mem = stk_data(&p->mem);

	if (pos >= stk_cap(&p->mem))
		icc(panic, "%s: OOB read [%ld].", __func__, pos);
	return (mem[pos]);
}

void
icc(proc_load, ICC(Proc *) p, char *code)
{
	while (*code)
	{
		char	*end;
		Word	n = strtoll(code, &end, 10);

		if (code == end)
			icc(panic, "failed to parse program.");
		if (!*end || *end == ',' || *end == '\n')
			code = end + 1;
		stk_push(&p->mem, n);
	}
}

void
icc(proc_run, ICC(Proc *) p)
{
	p->pc = 0;
	icc(runner_start, p);
}

void
icc(proc_link, ICC(Proc *) host, ICC(Proc *) target, ICC(LinkType) link)
{
	if (!host)
		icc(panic, " %s: no host process specified.", __func__);
	if (!target)
		icc(panic, " %s: no target process specified.", __func__);

	int	fd[2];

	if (pipe(fd) == -1)
		icc(panic, "%s: failed to link: %s.", __func__, strerror(errno));
	if (link == ICC_LINK_OUT)
	{
		host->fd_out = fd[WRITE_END];
		target->fd_in = fd[READ_END];
	}
	else
	{
		target->fd_out = fd[WRITE_END];
		host->fd_in = fd[READ_END];
	}
}
