/**
* runner.c
*/

#include <common.h>
#include <ops.h>
#include <runner.h>
#include <process.h>

static Word
icc(operand_mode, Word mode)
{
	return (mode % 10);
}

Word
icc(operand_get, ICC(Proc *) p, bool out)
{
	Word	mode = icc(operand_mode, p->mode);
	Word	val = icc(proc_read, p, p->pc++);

	if (mode & ICC_MODE_RELATIVE)
		val += p->rb;
	if (!(mode & ICC_MODE_IMMEDIATE) && !out)
		val = icc(proc_read, p, val);
	p->mode /= 10;

	return (val);
}

Word
icc(fetch, ICC(Proc *) p)
{
	if (p->pc >= p->mem.capacity)
		icc(panic, "PC out of the process memory. aborting.");

	Word	opcode = icc(proc_read, p, p->pc++);

	return (opcode);
}

void
icc(decode, ICC(Proc *) p, Word *p_opcode)
{
	Word	opcode = *p_opcode;
	Word	op = opcode % 100;
	Word	mode = opcode / 100;

	if (op > OP_hlt)
		icc(panic, "unknown op [%ld].", op);

	*p_opcode = op;
	p->mode = mode;
}

void
icc(execute, ICC(Proc *) p, Word opcode)
{
	icc(log, "executing: [%s]", ICC_OP_NAMES[opcode]);
	ICC(OP_TABLE)[opcode](p);
}

void
icc(runner_start, ICC(Proc *) p)
{
	while (1)
	{
		Word	opcode = icc(fetch, p);

		icc(decode, p, &opcode);
		if (opcode == OP_hlt)
			break ;
		icc(execute, p, opcode);
	}
}
