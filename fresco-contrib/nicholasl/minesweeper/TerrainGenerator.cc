#include "TerrainGenerator.hh"

int *cook_terrain(int width, int height, int mines) {
  assert(mines <= width * height);
  assert(width > 0);
  assert(height > 0);

  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  srand(tv.tv_sec);

  int *mine_field = (int*)calloc(sizeof(int), width*height);

  for (int i = 0; i < mines; i++) {
    int x = abs(rand() % width);
    int y = abs(rand() % height);
    while (*(mine_field+x+width*y) == 9) {
      x = abs(rand() % width);
      y = abs(rand() % height);
    }
    *(mine_field+x+width*y) = 9;
  }

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (*(mine_field+x+width*y) == 9) { continue; }
      int count = 0;
      if (x > 0) {
	if (*(mine_field+(x-1)+width*y) == 9) count++;	
	if (y > 0)
	  if (*(mine_field+(x-1)+width*(y-1)) == 9) count++;
	if (y < width-1)
	  if (*(mine_field+(x-1)+width*(y+1)) == 9) count++;
      }
      if (y > 0)
	if (*(mine_field+x+width*(y-1)) == 9) count++;
      if (y < width-1)
	if (*(mine_field+x+width*(y+1)) == 9) count++;
      if (x < height-1) {
	if (*(mine_field+(x+1)+width*y) == 9) count++;	
	if (y > 0)
	  if (*(mine_field+(x+1)+width*(y-1)) == 9) count++;
	if (y < width-1)
	  if (*(mine_field+(x+1)+width*(y+1)) == 9) count++;
      }
      *(mine_field+x+width*y) = count;
    }
  }

  return mine_field;
}
