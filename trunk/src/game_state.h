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
  Character *c; // the main character that will be controlled by player
  Map *map; // map object
  vector<Moveable *> moveables; // all moveable entities
  vector<Special *> specials; // all specials that can follow player
  vector<Drawable *> numbers; // numbers to place above specials
  unsigned int next_special; // num to determine pos in specials
  bool gravity; // is gravity on or off?
  bool collision; // is collision on or off?
  bool w, a, s, d; // are the arrows keys being pressed?
  bool last_x, last_y; // last directional keys pressed in x and y
  unsigned int map_slide_effect; // are we changing the view to another character?
  char last_key; // last key that was pressed
  char key_held; // used for activating specials ability/switching to special
  
public:
  Game_State(void);
  Game_State(Player *pl, Map *m, vector<Moveable *> mvs,
             vector<Special *> sps, vector<Drawable*> nums,
             FMOD_SYSTEM *system);
  
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
  
  // centers the map on the controlled character
  
  void pause_volume(void);
  void unpause_volume(void);
  
  // cleans all data from state
  void clean(void);
  
};

#endif // FLOCK__GAME_STATE__H
