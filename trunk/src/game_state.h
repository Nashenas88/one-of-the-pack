#ifndef FLOCK__GAME_STATE__H
#define FLOCK__GAME_STATE__H

#include <vector>
using namespace std;

#include "player.h"
#include "map.h"
#include "special.h"
#include "state.h"

class Game_State : public State
{
private:
  Player *p; // human controlled player
  Map *m; // map object
  vector<Drawable *> moveables; // all moveable entities
  vector<Special *> specials; // all specials that can follow player
  bool gravity; // is gravity on or off?
  bool collision; // is collision on or off?
  
public:
  Game_State(void);
  Game_State(Player *pl, Map *map, vector<Drawable *> mvs,
             vector<Special *> sps, FMOD_SYSTEM *system);
  
  // does all drawing for each level
  void draw(void);
  
  // handles a LOT of crap
  // collisions / movement / gravity
  void update(int &delta);
  
  // deals with keyboard presses
  void key_pressed(unsigned char key, int x, int y);
  // deals with keyboard releases
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  void pause_volume(void);
  void unpause_volume(void);
  
  // cleans all data from state
  void clean(void);
  
};

#endif // FLOCK__GAME_STATE__H
