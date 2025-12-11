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
	//icc(log, " PC = 0x%04lx executing: [%s]", p->pc, ICC_OP_NAMES[opcode]);
	ICC(OP_TABLE)[opcode](p);
}

bool
icc(changed_last, ICC(Proc *) p, Addr i)
{
	u8	bit_idx = i & 63;
	u32	idx = i >> 6;

	return (p->changed[idx] & (1ull << bit_idx));
}

void
icc(dump, ICC(Proc *) p)
{
	printf("\033c\033[0;0H");
	printf("               ");
	for (int i = 0; i < 8; ++i)
	{
		printf("                %02x", i);
		if (i == 7)
			printf("\n");
		else
			printf("|");
	}
	for (int i = 0; i < 256; ++i)
	{
		if (i % 8 == 0)
			printf("[ 0x%08x ] ", i);
		if (i == p->pc)
			printf("\033[32;1m[");
		else if (i == p->last_pc)
 			printf("\033[31;1m(");
		else
			printf(" \033[90m");
		if (icc(changed_last, p, i))
			printf("\033[34;1m");
		printf("%16ld", p->mem.items[i]);
		if (i == p->pc)
			printf("]");
		else if (i == p->last_pc)
 			printf(")");
		else
			printf(" ");
		if (i % 8 == 7)
			printf("\033[0m\n");
		else
			printf("\033[0m|");
	}
}

void
icc(runner_start, ICC(Proc *) p)
{
	p->changed = malloc((p->mem.capacity + 64) >> 6);
	memset(p->changed, 0, (p->mem.capacity + 64) >> 6);
//	printf("\033[?25l");

	while (1)
	{
		//icc(dump, p);
		memset(p->changed, 0, (p->mem.capacity + 64) >> 6);

		Addr	oldpc = p->pc;
		Word	opcode = icc(fetch, p);

		if (opcode == OP_hlt)
			break ;
		icc(decode, p, &opcode);
		icc(execute, p, opcode);

		p->last_pc = oldpc;

// 		char c;
// 		read(0, &c, 1);
	}
}
