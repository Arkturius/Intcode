/**
* runner.h
*/

#if !defined (_ICC_RUNNER_H)
# define _ICC_RUNNER_H

# include <common.h>
# include <process.h>

typedef enum	icc_operand_mode
{
	ICC_MODE_IMMEDIATE = 0b01,
	ICC_MODE_RELATIVE = 0b10,
}
ICC_OperandMode;

void
icc(runner_start, ICC(Proc *) p);

Word
icc(operand_get, ICC(Proc *) p, bool out);

#endif // _ICC_RUNNER_H
