#include <stdlib.h>
#include <time.h>

#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "graphics.h"
#include "game.h"

int main(int argc, char *argv[]) {

  srand(time(0));

  init_graphics();

  new_game();

  terminate_graphics();

  sceKernelExitProcess(0);
  return 0;
}
