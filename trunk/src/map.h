#ifndef FLOCK__MAP__H
#define FLOCK__MAP__H

#include <vector>
using namespace std;

#include "drawable.h"
#include "defines.h"

class Character;

#include "FMOD_includes.h"

enum
{
  M_TILE,
  M_COLL
};

class Special;
class Moveable;

class Map: public Drawable
{
private:
  // 2D array used to represent the map
  // represents both the tile images
  // and the collisions
  unsigned char ***map;
  // list of all types of tiles that will be used for drawing
  vector<Drawable *> tiles;
  vector<Moveable *> moves;
  vector<Special *> specs;
  vector<int> groups;
  
public:
  Map(void);
  Map(vector<Drawable *> t);
  
  // draws each item on the map
  void draw(void);
  
  // getting background
  Drawable *get_background(void) {return tiles.at((int)BG);}
  vector<Moveable *> get_moveables(void) {return moves;}
  void set_moveables(vector<Moveable*> m) {moves = m;}
  vector<Special *> get_specials(void) {return specs;}
  
  // simple methods for adding, getting, and removing blockers
  bool get_blocker(int x, int y) {return map[x][y][M_COLL] == 1;}
  tile_type get_tile(int x, int y) {return (tile_type) map[x][y][M_TILE];}
  tile_type get_coll(int x, int y) {return (tile_type) map[x][y][M_COLL];}
  bool make_rubber(int x, int y);
  bool return_from_rubber(int x, int y);
  void remove(int x, int y);
  void open_bouncer(int x, int y);
  void move(float x, float y);
  void convert_circuit(int x, int y);
  void unconvert_circuit(int x, int y);
  void clear_checkpoint(int x, int y);
  
  void get_goal(float &x, float &y);
  void breaker(int x, int y) {map[x][y][M_TILE] = BREAKABLE2;map[x][y][M_COLL] = BREAKING;}
  
  void update_breaks(void);
  
  void calculate_location(Drawable *d, int &x, int &y);
  
  // loads map from a binary image, returns false on fail, also modifies
  // moveables so that it includes a list of all objects that can move
  bool load_map(const char *map_bmp, vector<Moveable *> &moveables,
                vector<Special *> &specials, Texture *ts,
                vector<Texture*> texs, Character *player,
                FMOD_SYSTEM *system, vector<FMOD_SOUND *> musics,
                FMOD_CHANNEL *m_channel, vector<FMOD_SOUND *> effects,
                FMOD_CHANNEL *a_channel);
  
  void clean(void);
};

#endif // FLOCK__MAP__H
