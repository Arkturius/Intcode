/**
* grid.h
*/

#if !defined (_ICC_GRID_H)
# define _ICC_GRID_H

# include <common.h>

typedef u32	ICC(Cell);

typedef struct	icc_grid_s
{
	u32			w;
	u32			h;
	ICC(Cell *)	cells;
	u8			*dirty;
}
ICC(Grid);

# define	CLEAR_SCREEN		"\033[2J"
# define	HIDE_CURSOR			"\033[?25l"
# define	SHOW_CURSOR			"\033[?25h"
# define	GOTO_HOME			"\033[H"
# define	GOTO(y, x, sy, sx)	printf("\033[%d;%dH", (sy)+(y), (sx)+(x))

typedef enum	icc_cell_type_e
{
	CELL_SPACE,
	CELL_CORNER_UL,
	CELL_CORNER_UR,
	CELL_CORNER_DR,
	CELL_CORNER_DL,
	CELL_LINE_HOR,
	CELL_LINE_VER,
}
ICC(CellType);

void
icc(grid_init, ICC(Grid *) grid);

void
icc(grid_destroy, ICC(Grid *) grid);

void
icc(grid_render, ICC(Grid *) grid);

void
icc(grid_layout, ICC(Grid *) g);

void
icc(grid_put_text, ICC(Grid *) g, const char *s, u32 x, u32 y, u32 pad);

#endif // _ICC_GRID_H
