/**
* ops.h
*/

#if !defined (_ICC_OPS_H)
# define _ICC_OPS_H

# include <common.h>
# include <core.h>

# define	_CONCAT(a, b)	a ## b
# define	CONCAT(a, b)	_CONCAT(a, b)

# define	ICC_OPS(_x)														\
	_x(add,  1)																\
	_x(mul,  2)																\
	_x(in,   3)																\
	_x(out,  4)																\
	_x(jnz,  5)																\
	_x(jz,   6)																\
	_x(clt,  7)																\
	_x(ceq,  8)																\
	_x(rel,  9)																\
	_x(hlt, 99)																\

# define	ICC_OPS_NAME(_name)												\
	CONCAT(OP_, _name)

# define	ICC_OPS_ENUM_DECL(_name, _code)									\
	ICC_OPS_NAME(_name) = _code,

# define	ICC_OPS_FUNC_DECL(_name, _code)									\
	[ICC_OPS_NAME(_name)] = ICC(_name),

# define	ICC_OPS_FUNC_PROT(_name, _code)									\
	void	icc(_name, ICC(Proc *) p);

ICC_OPS(ICC_OPS_FUNC_PROT)

typedef void	(*icc_op_fn)(ICC(Proc *));

# define	ICC_OPS_NAMES(_name, _code)										\
	[ICC_OPS_NAME(_name)] = #_name,

extern const icc_op_fn	ICC(OP_TABLE)[];
extern const char		*ICC(OP_NAMES)[];

typedef enum	icc_opcode_e
{
	ICC_OPS(ICC_OPS_ENUM_DECL)
}
ICC_Opcode;

#endif // _ICC_OPS_H
