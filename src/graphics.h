#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vita2d.h>
#include <psp2/pgf.h>
#include "vector.h"

void init_graphics(void);
void terminate_graphics(void);
void setDims(int bColumns, int bRows);
void start_frame(void);
void end_frame(void);
void setSpacings(void);
void draw_background(void);
void draw_islands(int board_tiles[15][8], int board_values[15][8]);
void draw_bridges(int board_tiles[15][8]);
void draw_selected_point(int board_tiles[15][8], board_vector selected_point);
void draw_frame(int board_tiles[15][8], int board_values[15][8], board_vector selected_point);

#endif
