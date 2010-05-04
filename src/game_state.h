#ifndef FLOCK__GAME_STATE__H
#define FLOCK__GAME_STATE__H

#include <vector>
using namespace std;

#include "player.h"
#include "map.h"
#include "special.h"
#include "state.h"
#include "beam.h"

class Game_State : public State
{
private:
  Player *p; // human controlled player
  Character *c; // the main character that will be controlled by player
  Map *map; // map object
  vector<Moveable *> moveables; // all moveable entities
  vector<Special *> specials; // all specials that can follow player
  vector<Drawable *> numbers; // numbers to place above specials
  vector<Beam *> beams; // all beams flying in the air
  vector<Beam *> kisses; // paris's kisses
  unsigned int next_special; // num to determine pos in specials
  bool gravity; // is gravity on or off?
  bool collision; // is collision on or off?
  bool w, a, s, d; // are the arrows keys being pressed?
  bool last_x, last_y; // last directional keys pressed in x and y
  unsigned int map_slide_effect; // are we changing the view to another character?
  char last_key; // last key that was pressed
  char key_held; // used for activating specials ability/switching to special
  int jump_delta;
  bool controllable;
  bool debug;
  
public:
  Game_State(void);
  Game_State(Player *pl, Map *m, vector<Moveable *> mvs,
             vector<Special *> sps, vector<Drawable*> nums,
             FMOD_SYSTEM *system);
  
  Player *get_player(void) {return p;}
  vector<Special *> get_specials(void) {return specials;}
  vector<Moveable *> get_moveables(void) {return moveables;}
  Map *get_map(void) {return map;}
  
  // does all drawing for each level
  void draw(void);
  
  // this is basically the game
  // it should probably be split up into more functions
  void update(int &delta);
  
  // keyboard functions
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  // changes the volume, should be used when
  // entering and exiting the pause state
  void pause_volume(void);
  void unpause_volume(void);
  
  void reset_keys(void);
  
  // cleans all data from state
  void clean(void);
  
};

#endif // FLOCK__GAME_STATE__H
