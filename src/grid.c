/**
* grid.c
*/

#include <common.h>
#include <grid.h>

#include <sys/ioctl.h>

# define UBSHL(b, s)	(((u8)(b)) << s)
# define BTOI(s)		s[0] | UBSHL(s[1], 8) | UBSHL(s[2], 16) | UBSHL(s[3], 24)

static const char	*ICC(CELL_TABLE)[] =
{
	[CELL_SPACE]     = " \x00\x00",
	[CELL_CORNER_UL] = "╭",
	[CELL_CORNER_UR] = "╮",
	[CELL_CORNER_DR] = "╯",
	[CELL_CORNER_DL] = "╰",
	[CELL_LINE_HOR]  = "─",
	[CELL_LINE_VER]  = "│",
};

static void	
icc(grid_size, uint32_t *w, uint32_t *h)
{
	struct winsize	win;

	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	*h = win.ws_row;
	*w = win.ws_col;
}

static inline bool
icc(cell_is_dirty, ICC(Grid *) g, u32 idx)
{
	u32	byte = idx >> 3;
	u8	bit = idx & 7;

	return (g->dirty[byte] & (1 << bit));
}

static inline void
icc(cell_set_dirty, ICC(Grid *) g, u32 idx)
{
	u32	byte = idx >> 3;
	u8	bit = idx & 7;

	g->dirty[byte] |= (1 << bit);
}

static inline void
icc(cell_set_clean, ICC(Grid *) g, u32 idx)
{
	u32	byte = idx >> 3;
	u8	bit = idx & 7;

	g->dirty[byte] &= ~(1 << bit);
}

# define	SHL(x, n)		((x) >> (n))
# define	AND(x, n)		((x) & (n))
# define	OR(x, n)		((x) | (n))
# define	SLA(x, s, a)	AND(SHL(x, s), a)

static inline void
icc(grid_put_utf8, ICC(Cell) cp)
{
	char	c[5] = {0};
	memcpy(c, &cp, strlen((char *)&cp));
	printf("%s", c);
	return;

	char	*start = &c[3];

	if (cp < 0x80)
		*start-- = cp;
	else
	{
		*start-- = OR(0x80, AND(cp, 0x3f));
		if (cp < 0x800)
		{
			*start-- = OR(0xc0, SHL(cp, 6));
			goto render;
		}
		*start-- = OR(0x80, SLA(cp, 6, 0x3f));
		if (cp < 0x10000)
		{
			*start-- = OR(0xe0, SHL(cp, 12));
			goto render;
		}
		*start-- = OR(0x80, SLA(cp, 12, 0x3f));
		*start-- = OR(0xf0, SHL(cp, 18));
	}
render:
	++start;
	puts(start);
}

void
icc(grid_render, ICC(Grid *) g)
{
	icc(grid_put_text, g, "Intcode computer", 2, 2, 0);
	GOTO(0, 0, 0, 0);
	for (u32 y = 0; y < g->h; ++y)
	{
		for (u32 x = 0; x < g->w; ++x)
		{
			u32	idx = y * g->w + x;

			//dprintf(2, "char %d at [x(%u), y(%u)]\n", g->cells[idx], x, y);
			//GOTO(y, x, 1, 1);
			if (icc(cell_is_dirty, g, idx))
			{
				icc(grid_put_utf8, g->cells[idx]);
				icc(cell_set_clean, g, idx);
			}
		}
	}
	fflush(stdout);
}

void
icc(grid_put_text, ICC(Grid *) g, const char *s, u32 x, u32 y, u32 pad)
{
	u32	i;
	u32	len = strlen(s);

	GOTO(x, y, 1, 1);
	for (i = 0; i < len && i + x < g->w - 1; ++i)
		icc(grid_put_utf8, (u32)((u8)s[i]));
	for (u32 j = 0; j < pad && j + i + x < g->w - 1; ++j)
		icc(grid_put_utf8, ' ');
}

void
icc(grid_layout, ICC(Grid *) g)
{
	g->cells[0] = *(u32 *)ICC(CELL_TABLE)[CELL_CORNER_UL];
	g->cells[g->w - 1] = *(u32 *)ICC(CELL_TABLE)[CELL_CORNER_UR];
	g->cells[(g->h - 1) * g->w] = *(u32 *)ICC(CELL_TABLE)[CELL_CORNER_DL];
	g->cells[g->w * g->h - 1] = *(u32 *)ICC(CELL_TABLE)[CELL_CORNER_DR];
	for (u32 x = 1; x < g->w - 1; ++x)
	{
		g->cells[x] = *(u32 *)ICC(CELL_TABLE)[CELL_LINE_HOR];
		g->cells[(g->h - 1) * g->w + x] = *(u32 *)ICC(CELL_TABLE)[CELL_LINE_HOR];
	}
	for (u32 y = 1; y < g->h - 1; ++y)
	{
		g->cells[y * g->w] = *(u32 *)ICC(CELL_TABLE)[CELL_LINE_VER];
		g->cells[(y + 1) * g->w - 1] = *(u32 *)ICC(CELL_TABLE)[CELL_LINE_VER];
	}
	for (u32 y = 1; y < g->h - 1; ++y)
	{
		for (u32 x = 1; x < g->w - 1; ++x)
		{
			u32	idx = y * g->w + x;

			g->cells[idx] = ' ';
		}
	}
}

void
icc(grid_init, ICC(Grid *) g)
{
	if (!g)
		icc(panic, "no grid pointer.");

	icc(grid_size, &g->w, &g->h);

	u32	cell_count = g->h * g->w;
	
	g->cells = malloc(cell_count * sizeof(ICC(Cell)));
	g->dirty = malloc((cell_count >> 3) * sizeof(char));

	for (u32 i = 0; i < cell_count; ++i)
		g->cells[i] = '#';
	memset(g->dirty, 0xff, (cell_count >> 3) * sizeof(char));
}
