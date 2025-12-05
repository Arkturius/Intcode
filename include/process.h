/**
* process.h
*/

#if !defined (_ICC_PROCESS)
# define _ICC_PROCESS

# include <common.h>

# define	READ_END	0
# define	WRITE_END	0

typedef enum	icc_linktype_e
{
	ICC_LINK_IN,
	ICC_LINK_OUT,
}
ICC_LinkType;

typedef struct	icc_proc_s
{
	Addr	pc;
	Addr	rb;
	Word	mode;
	Words	mem;
	int		fd_in;
	int		fd_out;
	char	*stream_in;
	char	*stream_out;
}
ICC_Proc;

stk(ICC(Proc), ICC(Procs));

void
icc(proc_init, char *code);

void
icc(proc_start, ICC(Proc *), Word pc);

void
icc(proc_stop,  ICC(Proc *));

void
icc(proc_write, ICC(Proc *), Addr pos, Word n);

Word
icc(proc_read,  ICC(Proc *), Addr pos);

void
icc(proc_load,  ICC(Proc *), char *code);

void
icc(proc_run,   ICC(Proc *));

void
icc(proc_link,  ICC(Proc *), ICC(Proc *), ICC(LinkType));

#endif // _ICC_PROCESS
