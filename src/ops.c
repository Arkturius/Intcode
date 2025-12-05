/**
* ops.c
*/

#include <intcode.h>
#include <unistd.h>

const icc_op_fn	ICC(OP_TABLE)[] = 
{
	ICC_OPS(ICC_OPS_FUNC_DECL)
};

const char	*ICC(OP_NAMES)[] = 
{
	ICC_OPS(ICC_OPS_NAMES)
};

void
icc(add, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);
	Word	c = icc(operand_get, p, true);

	icc(proc_write, p, c, a + b);
}

void
icc(mul, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);
	Word	c = icc(operand_get, p, true);

	icc(proc_write, p, c, a * b);
}

void
icc(in, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, true);
	Word	val = 0;

	char	*end;
	char	buffer[32] = {0};
	int		r = 0;

	if (!p->stream_in)
	{
		do
		{
			r = read(p->fd_in, buffer, sizeof(buffer) - 1);
			if (r < 0)
				icc(panic, "read error.");
		}
		while (r == sizeof(buffer) - 1);
		p->stream_in = buffer;
	}

	errno = 0;
	val = strtol(p->stream_in, &end, 10);
	if (errno == ERANGE)
		icc(panic, "strtol conversion.");

	if (end - buffer != r)
		p->stream_in = strdup(end);

	icc(proc_write, p, a, val);
}

void
icc(out, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);

	char	buffer[32] = {0};

	snprintf(buffer, 31, "%ld", a);
	write(p->fd_out, buffer, strlen(buffer));

	icc(log, "OUTPUT -> %ld", a);
}

void
icc(jnz, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);

	if (a)
		p->pc = b;
}

void
icc(jz, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);

	if (!a)
		p->pc = b;
}

void
icc(clt, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);
	Word	c = icc(operand_get, p, true);

	icc(proc_write, p, c, (a < b));
}

void
icc(ceq, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);
	Word	b = icc(operand_get, p, false);
	Word	c = icc(operand_get, p, true);

	icc(proc_write, p, c, (a == b));
}

void
icc(rel, ICC(Proc *) p)
{
	Word	a = icc(operand_get, p, false);

	p->rb += a;
}

void
icc(hlt, ICC(Proc *) p)
{
	(void) p;

	core.running = false;
	return ;
}
