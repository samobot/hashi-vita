#include <stdio.h>

#include <vita2d.h>
#include <psp2/pgf.h>

#include "tiles.h"
#include "vector.h"
#include "graphics.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544
#define BG_COLOR RGBA8(64, 64, 64, 255)
#define BG_LINE_COLOR RGBA8(102, 102, 102, 255)
#define CIRCLE_COLOR RGBA8(127, 127, 127, 255)
#define CIRCLE_RING_COLOR RGBA8(255, 255, 255, 255)
#define CIRCLE_RING_COLOR_INVALID RGBA8(255, 0, 0, 255)
#define CIRCLE_RING_COLOR_CORRECT RGBA8(0, 255, 0, 255)
#define TEXT_COLOR RGBA8(255, 255, 255, 255)
#define BRIDGE_COLOR RGBA8(255, 255, 255, 255)
#define SELECTOR_COLOR RGBA8(65, 102, 236, 255)

int x_space;
int y_space;

int gRows;
int gColumns;

vita2d_pgf *pgf;

void init_graphics() {
  vita2d_init();
  vita2d_set_clear_color(BG_COLOR);
  pgf = vita2d_load_default_pgf();
}

void terminate_graphics() {
  vita2d_fini();
  vita2d_free_pgf(pgf);
}

void setDims(int bColumns, int bRows) {
  gRows = bRows;
  gColumns = bColumns;
}

void start_frame() {
  vita2d_start_drawing();
  vita2d_clear_screen();
}

void end_frame() {
  vita2d_end_drawing();
  vita2d_swap_buffers();
}

void setSpacings() {
  x_space = SCREEN_WIDTH/(gColumns+1);
  y_space = (SCREEN_HEIGHT-4)/(gRows+1);
}

void draw_background() {
	int i=1;
	const int INITX = (SCREEN_WIDTH/(gColumns+1));
	const int INITY = ((SCREEN_HEIGHT-4)/(gRows+1))+2;
	for(i=1; i < gRows+1; i++) {
		int spacedI = (i*y_space)+2;
		vita2d_draw_line(INITX, spacedI, (SCREEN_WIDTH-INITX), spacedI, BG_LINE_COLOR);
	}
	for(i=1; i < gColumns+1; i++) {
		int spacedI = (i*x_space);
		vita2d_draw_line(spacedI, INITY, spacedI, (SCREEN_HEIGHT-INITY), BG_LINE_COLOR);
	}
}

void draw_islands(int board_tiles[15][8], int board_values[15][8]) {
  for(int x=0; x < gColumns; x++) {
    for(int y=0; y < gRows; y++) {
      if(board_tiles[x][y] == island) {
        int connected_pieces = 0;
        for(int z=-1; z<=1; z+=2) {
          if(x+z < gColumns && x+z >= 0) {
            if(board_tiles[x+z][y] == horiz_bridge) {
              connected_pieces+=1;
            } else if(board_tiles[x+z][y] == horiz_double_bridge) {
              connected_pieces+=2;
            }
          }
          if(y+z < gRows && y+z >= 0) {
            if(board_tiles[x][y+z] == vert_bridge) {
              connected_pieces+=1;
            } else if(board_tiles[x][y+z] == vert_double_bridge) {
              connected_pieces+=2;
            }
          }
        }
        if(connected_pieces == board_values[x][y]) {
          vita2d_draw_fill_circle(((x+1)*x_space)-1, ((y+1)*y_space)+2, 25, CIRCLE_RING_COLOR_CORRECT);
        } else if(connected_pieces > board_values[x][y]) {
          vita2d_draw_fill_circle(((x+1)*x_space)-1, ((y+1)*y_space)+2, 25, CIRCLE_RING_COLOR_INVALID);
        } else {
          vita2d_draw_fill_circle(((x+1)*x_space)-1, ((y+1)*y_space)+2, 25, CIRCLE_RING_COLOR);
        }
        vita2d_draw_fill_circle(((x+1)*x_space)-1, ((y+1)*y_space)+2, 23, CIRCLE_COLOR);
        vita2d_pgf_draw_textf(pgf, ((x+1)*x_space)-7, ((y+1)*y_space)+8, TEXT_COLOR, 1.0f, "%d", board_values[x][y]);
      }
    }
  }
}

void draw_bridges(int board_tiles[15][8]) {
  for(int x=0; x < gColumns; x++) {
    for(int y=0; y < gRows; y++) {
      switch(board_tiles[x][y]) {
        case vert_bridge:
          vita2d_draw_line(((x+1)*x_space)-1, ((y)*y_space)+2, ((x+1)*x_space)-1, ((y+2)*y_space)+2, BRIDGE_COLOR);
          break;
        case vert_double_bridge:
          vita2d_draw_line(((x+1)*x_space)-1-5, ((y)*y_space)+2, ((x+1)*x_space)-1-5, ((y+2)*y_space)+2, BRIDGE_COLOR);
          vita2d_draw_line(((x+1)*x_space)-1+5, ((y)*y_space)+2, ((x+1)*x_space)-1+5, ((y+2)*y_space)+2, BRIDGE_COLOR);
          break;
        case horiz_bridge:
          vita2d_draw_line(((x)*x_space)-1, ((y+1)*y_space)+2, ((x+2)*x_space)-1, ((y+1)*y_space)+2, BRIDGE_COLOR);
          break;
        case horiz_double_bridge:
        vita2d_draw_line(((x)*x_space)-1, ((y+1)*y_space)+2-5, ((x+2)*x_space)-1, ((y+1)*y_space)+2-5, BRIDGE_COLOR);
        vita2d_draw_line(((x)*x_space)-1, ((y+1)*y_space)+2+5, ((x+2)*x_space)-1, ((y+1)*y_space)+2+5, BRIDGE_COLOR);
        break;
        case no_tile:
        break;
      }
    }
  }
}

void draw_selected_point(int board_tiles[15][8], board_vector selected_point) {
  if(board_tiles[selected_point.x][selected_point.y] == island) {
    vita2d_draw_fill_circle(((selected_point.x+1)*x_space)-1, ((selected_point.y+1)*y_space)+2, 27, SELECTOR_COLOR);
  } else {
    vita2d_draw_fill_circle(((selected_point.x+1)*x_space)-1, ((selected_point.y+1)*y_space)+2, 10, SELECTOR_COLOR);
  }
}

void draw_frame(int board_tiles[15][8], int board_values[15][8], board_vector selected_point) {
  draw_background();
  draw_bridges(board_tiles);
  draw_selected_point(board_tiles, selected_point);
  draw_islands(board_tiles, board_values);
}
