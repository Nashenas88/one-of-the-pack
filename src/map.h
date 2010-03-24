#ifndef FLOCK__MAP__H
#define FLOCK__MAP__H

#include <vector>
using namespace std;

#include "drawable.h"
#include "character.h"
#include "defines.h"

#include "FMOD_includes.h"

enum
{
  M_TILE,
  M_COLL
};

class Map: public Drawable
{
private:
  // 2D array used to represent the map
  // represents both the tile images
  // and the collisions
  unsigned char ***map;
  // list of all types of tiles that will be used for drawing
  vector<Drawable *> tiles;
  
public:
  Map(void);
  Map(vector<Drawable *> t);
  
  // draws each item on the map
  void draw(void);
  
  // getting background
  Drawable *get_background(void) {return tiles.at((int)BG);}
  
  // simple methods for adding, getting, and removing blockers
  bool get_blocker(int x, int y) {return map[x][y][M_COLL]==1?true:false;}
  tile_type get_tile(int x, int y) {return (tile_type) map[x][y][M_TILE];}
  
  // loads map from a binary image, returns false on fail, also modifies
  // moveables so that it includes a list of all objects that can move
  bool load_map(const char *map_bmp, vector<Drawable *> &moveables, vector<Character *> &specials, vector<Texture*> texs,
                FMOD_SYSTEM *system, vector<FMOD_SOUND *> sounds,
                FMOD_CHANNEL *channel);
};

#endif // FLOCK__MAP__H
