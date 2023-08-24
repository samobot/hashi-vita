#include <psp2/ctrl.h>
#include <psp2/touch.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tiles.h"
#include "game.h"
#include "graphics.h"
#include "vector.h"

#define ROWS 8
#define COLUMNS 15
#define ISLANDS 25
#define MAXFAIL 500

int board_tiles[COLUMNS][ROWS] = { 0 };
int board_values[COLUMNS][ROWS] = { 0 };
board_vector selected_point = {0, 0};

board_vector rand_point() {
  board_vector point;
  point.x = rand() % COLUMNS;
  point.y = rand() % ROWS;
  return point;
}

int bridge_count(board_vector point) {
  int c = 0;
  for(int i = -1; i <= 1; i+=2) {
    if(point.x+i < COLUMNS && point.x+i >= 0) { 
      switch(board_tiles[point.x+i][point.y]) {
        case horiz_bridge:
          c+=1;
          break;
        case horiz_double_bridge:
          c+=2;
          break;
      }
    }
    if(point.y+i < ROWS && point.y+i >= 0) {
      switch(board_tiles[point.x][point.y+i]) {
        case vert_bridge:
          c+=1;
          break;
        case vert_double_bridge:
          c+=2;
          break;
      }
    }
  }
  return c;
}

void line_replace(int x, int y, board_vector direction, int tile) {
  int offsetX = direction.x;
  int offsetY = direction.y;
  while(board_tiles[x+offsetX][y+offsetY] != island) {
    board_tiles[x+offsetX][y+offsetY] = tile;
    offsetX += direction.x;
    offsetY += direction.y;
    if((x+offsetX <= 0 || x+offsetX >= COLUMNS) && direction.x != 0) return;
    if((y+offsetY <= 0 || y+offsetY >= ROWS) && direction.y != 0) return;
  }
}

int try_draw_line(int x, int y, board_vector direction) {

  if((direction.x == 0 && direction.y == 0) || (direction.x != 0 && direction.y != 0)) return 1; //make sure that direction vector only goes straight up or down
  if(board_tiles[x][y] != island) return 1; //make sure that the origin point is an island
  if(board_tiles[x+direction.x][y+direction.y] == island) return 1; //ensure the very next tile isn't another island since no bridge can be draw between them

  printf("direction check successful\n");

  int offsetX = direction.x;
  int offsetY = direction.y;

  //check if all items in the line match until an island is reached

  while(board_tiles[x+offsetX][y+offsetY] != island) {
    printf("starting check\n");
    if(!(board_tiles[x+offsetX][y+offsetY] == board_tiles[x+offsetX+direction.x][y+offsetY+direction.y])) {  //Check if the adjacent spaces are the same tile (none or 1-bridge or 2-bridge)
      printf("vals: %d %d\n", board_tiles[x+offsetX][y+offsetY], board_tiles[x+offsetX+direction.x][y+offsetY+direction.y]);
      printf("vals: (%d, %d) (%d, %d)\n", x+offsetX, y+offsetY, x+offsetX+direction.x, y+offsetY+direction.y);
      if(!((offsetX+direction.x) == 0 || (offsetY+direction.y) == 0)) {printf("failed at first check\n"); return 1;}; //make sure that equality check doesn't trip on the first island
    }
    offsetX += direction.x;
    offsetY += direction.y;
    if(x+offsetX < 0 || x+offsetX > COLUMNS) {printf("failed at second check\n");return 1;}  //return if outside of boundries to prevent an error
    if(y+offsetY < 0 || y+offsetY > ROWS) {printf("failed at third check\n");return 1;} //^
    printf("(%d, %d) successful\n", x, y);
  }

  //check whether all the items are no tile, 1-bridge or, 2-bridge and change board accordingly

  printf("line of tiles = %d\n", board_tiles[x+direction.x][y+direction.y]);

  switch (board_tiles[x+direction.x][y+direction.y]) {
    case no_tile:
      line_replace(x, y, direction, (direction.x == 0 ? vert_bridge : horiz_bridge));
      break;
    case vert_bridge:
      line_replace(x, y, direction, vert_double_bridge);
      break;
    case vert_double_bridge:
      line_replace(x, y, direction, no_tile);
      break;
    case horiz_bridge:
      line_replace(x, y, direction, horiz_double_bridge);
      break;
    case horiz_double_bridge:
      line_replace(x, y, direction, no_tile);
      break;
    case island:
      return 1;
  }

  return 0;
}

int try_gen_bridge_island(int x, int y, board_vector direction, int distance, int tile) {
  for(int i=1; i <= distance; i++) {
    if(board_tiles[x+(direction.x*i)][y+(direction.y*i)] != no_tile) {
      return 1;
    }
  }
  board_tiles[x+(distance*direction.x)][y+(distance*direction.y)] = island;
  for(int i=1; i < distance; i++) {
    board_tiles[x+(direction.x*i)][y+(direction.y*i)] = tile;
  }
  return 0;
}

void create_board(int islands) {
  /*for(int x=0; x < COLUMNS; x++) {
    for(int y=0; y < ROWS; y++ ) {
      printf("(%d, %d): %d\n", x, y, board_tiles[x][y]);
    }
  }*/
  setDims(COLUMNS, ROWS);
  setSpacings();
  board_vector added_points[islands];
  board_vector first_point = rand_point();
  added_points[0] = first_point;
  board_tiles[first_point.x][first_point.y] = island;
  board_values[first_point.x][first_point.y] = 69;
  int successes = 1;
  int fails = 0;
  while(successes < islands) {
    board_vector extend_point = added_points[rand() % successes];
    //board_vector extend_point = first_point;
    board_vector direction;
    direction.x = 0;
    direction.y = 0;
    int MAXDIST = 0;
    switch(rand() % 4) {
      case 0:
        direction.x = -1;
        MAXDIST = extend_point.x;
        break;
      case 1:
        direction.x = 1;
        MAXDIST = (COLUMNS - 1) - extend_point.x;
        break;
      case 2:
        direction.y = -1;
        MAXDIST = extend_point.y;
        break;
      case 3:
        direction.y = 1;
        MAXDIST = (ROWS - 1) - extend_point.y;
        break;
    }
    if(MAXDIST == 0) continue;
    int double_bridge = (rand() % 2);
    int distance = (rand() % MAXDIST) + 1;
    if(distance <= 1) continue;
    int tile = island;
    if(direction.x == 0) {
      if(double_bridge != 0) {
        tile = vert_double_bridge;
      } else {
        tile = vert_bridge;
      }
    } else {
      if(double_bridge != 0) {
        tile = horiz_double_bridge;
      } else {
        tile = horiz_bridge;
      }
    }
    int pass = try_gen_bridge_island(extend_point.x, extend_point.y, direction, distance, tile);
    if(pass == 0) {
      successes++;
      printf("(%d, %d) (%d, %d) %d\n", extend_point.x, extend_point.y, direction.x, direction.y, distance);
      board_vector new_point;
      new_point.x = extend_point.x+(direction.x*distance);
      new_point.y = extend_point.y+(direction.y*distance);
      added_points[successes-1] = new_point;
      printf("added points: ");
      for(int q = 0; q < successes; q++) {
        printf("(%d, %d), ", added_points[q].x, added_points[q].y);
      }
      printf("\n");
    } else {
      fails++;
      if(fails == MAXFAIL) {
        successes = islands;
      }
    }
  }
  printf("fails: %d\n", fails);
  for(int i = 0; i < islands; i++) {
    if(board_tiles[added_points[i].x][added_points[i].y] == island) {
      board_values[added_points[i].x][added_points[i].y] = bridge_count(added_points[i]);
    }
  }
  for(int dx = 0; dx < COLUMNS; dx++) {
    for(int dy = 0; dy < ROWS; dy++) {
      if(board_tiles[dx][dy] != island) {
        board_tiles[dx][dy] = no_tile;
      }
    }
  }
  /*board_tiles[1][1] = island;
  board_values[1][1] = 2;
  board_tiles[1][3] = island;
  board_values[1][3] = 3;
  board_tiles[3][1] = island;
  board_values[3][1] = 3;
  board_tiles[3][3] = island;
  board_values[3][3] = 4;
  board_tiles[4][1] = island;
  board_values[4][1] = 2;
  board_tiles[4][3] = island;
  board_values[4][2] = 2;
  board_vector direction;
  direction.x = 1;
  direction.y = 0;
  try_draw_line(1, 1, direction);
  direction.x = 0;
  direction.y = 1;
  try_draw_line(1, 1, direction);
  direction.x = -1;
  direction.y = 0;
  try_draw_line(3, 1, direction);
  direction.x = 0;
  direction.y = -1;
  try_draw_line(1, 3, direction);*/
  /*board_tiles[4][4] = vert_double_bridge;
  board_tiles[1][2] = vert_bridge;
  board_tiles[3][2] = vert_double_bridge;
  board_tiles[2][1] = horiz_bridge;
  board_tiles[2][3] = horiz_double_bridge;*/
}

int check_board() {
  for(int y = 0; y < ROWS; y++) {
    for(int x = 0; x < COLUMNS; x++) {
      int connected_pieces = 0;
      for(int z=-1; z<=1; z+=2) {
        if(board_tiles[x+z][y] == horiz_bridge) {
          connected_pieces+=1;
        } else if(board_tiles[x+z][y] == horiz_double_bridge) {
          connected_pieces+=2;
        }
        if(board_tiles[x][y+z] == vert_bridge) {
          connected_pieces+=1;
        } else if(board_tiles[x][y+z] == vert_double_bridge) {
          connected_pieces+=2;
        }
      }
      if(connected_pieces != board_values[x][y]) return 0;
    }
  }
  //hug the left wall
  return 1;
}

void handleButtons(SceCtrlData old_ctrl, SceCtrlData ctrl) {
  if(old_ctrl.up == 0 && ctrl.up != 0) {
    selected_point.y = selected_point.y-1;
  }
  if(old_ctrl.down == 0 && ctrl.down != 0) {
    selected_point.y = selected_point.y+1;
  }
}

void new_game() {
  create_board(ISLANDS);

  selected_point.x = 0;
  selected_point.y = 0;



  sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
  SceCtrlData old_ctrl;
  SceCtrlData ctrl;

  //sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
  //sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);

  do {
    old_ctrl = ctrl;
    sceCtrlPeekBufferPositive(0, &ctrl, 1);

    /*
    if(ctrl.buttons == SCE_CTRL_UP) {
      printf("%s", check_board() ? "solved\n" : "unsolved\n");
    }
    */

    if(ctrl.buttons != old_ctrl.buttons) {
      printf("buttons changed");
      if(ctrl.buttons & SCE_CTRL_UP) selected_point.y = selected_point.y-1;
      if(ctrl.buttons & SCE_CTRL_DOWN) selected_point.y = selected_point.y+1;
      if(ctrl.buttons & SCE_CTRL_LEFT) selected_point.x = selected_point.x-1;
      if(ctrl.buttons & SCE_CTRL_RIGHT) selected_point.x = selected_point.x+1;
      if(ctrl.buttons & SCE_CTRL_TRIANGLE) try_draw_line(selected_point.x, selected_point.y, (board_vector){0, -1});
      if(ctrl.buttons & SCE_CTRL_CROSS) try_draw_line(selected_point.x, selected_point.y, (board_vector){0, 1});
      if(ctrl.buttons & SCE_CTRL_SQUARE) try_draw_line(selected_point.x, selected_point.y, (board_vector){-1, 0});
      if(ctrl.buttons & SCE_CTRL_CIRCLE) try_draw_line(selected_point.x, selected_point.y, (board_vector){1, 0});
      if(ctrl.buttons & SCE_CTRL_LTRIGGER) {
        printf("selected: (%d, %d)", selected_point.x, selected_point.y);
      }
    }

    start_frame();
    draw_frame(board_tiles, board_values, selected_point);
    end_frame();
  } while (ctrl.buttons != (SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER));

}
