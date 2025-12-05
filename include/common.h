/**
* common.h
*/

#if !defined (_ICC_COMMON)
# define _ICC_COMMON

# include <unistd.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <stdbool.h>

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;

typedef int64_t		i64;
typedef int32_t		i32;
typedef int16_t		i16;
typedef int8_t		i8;

typedef uintptr_t	uptr;
typedef intptr_t	iptr;

# define	array_size(_a)	(sizeof(_a)/sizeof((_a)[0]))

# define	stk_type(_name)		typedef struct icc_stk_##_name##_s
# define	stk_decl(_type)		_type *items; u32 size; u32 capacity;
# define	stk(_type, _name)	stk_type(_name) { stk_decl(_type) } _name;

# define	stk_data(s)	(s)->items
# define	stk_size(s)	(s)->size
# define	stk_cap(s)	(s)->capacity

# define	stk_push(s, e)													\
	({																		\
		if (stk_size(s) + 1 >= stk_cap(s))									\
		{																	\
			stk_cap(s) = stk_cap(s) ? stk_cap(s) << 1 : 64;					\
			stk_data(s) = realloc											\
			(																\
				stk_data(s),												\
				stk_cap(s) * sizeof(stk_data(s)[0])							\
			);																\
		}																	\
		stk_data(s)[stk_size(s)++] = (e);									\
		&stk_data(s)[stk_size(s) - 1];										\
	})

# define	stk_pop(s)														\
	({																		\
		stk_size(s) -= (stk_size(s) > 0);									\
		stk_data(s)[stk_size(s)];											\
	})


typedef i64	Addr;
typedef i64	Word;

stk(Word, Words);
stk(Addr, Addrs);

# define	ICC(x)				ICC_##x
# define	ICC_DECL(x, ...)	ICC(x)( __VA_ARGS__ )
# define	icc(x, ...)			ICC(x)( __VA_ARGS__ )

static void
icc(panic_handler)
{
	exit(1);
}

# define	ICC_panic(fmt, ...)												\
																			\
	{																		\
		dprintf(2, "[X] " fmt "\n", ##__VA_ARGS__);							\
		ICC_panic_handler();												\
	}

# define	ICC_log(fmt, ...)												\
																			\
	dprintf(2, "[+] " fmt "\n", ##__VA_ARGS__);

# define	ICC_err(fmt, ...)												\
																			\
	dprintf(2, "[-] " fmt "\n", ##__VA_ARGS__);

#endif // _ICC_COMMON
