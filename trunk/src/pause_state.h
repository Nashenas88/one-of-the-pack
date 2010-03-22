#ifndef FLOCK__PAUSE_STATE__H
#define FLOCK__PAUSE_STATE__H

#include <vector>
using namespace std;

#include "drawable.h"
#include "FMOD_includes.h"
#include "state.h"
#include "game_state.h"

class Pause_State : public State
{
private:
  FMOD_DSP *dspparameq;
  Game_State *game_state;
  vector<Drawable *> items;
  Drawable *background;
  Drawable *map;
  Drawable *pointer;
  unsigned int selected;
public:
  Pause_State(void);
  Pause_State(FMOD_SYSTEM *system, FMOD_DSP *pe, Game_State *gs,
              vector<Drawable *> i, Drawable *b, Drawable *m, Drawable *p);
  
  void draw(void);
  
  void update(int &delta);
  
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  
  void clean(void);
};
#endif // FLOCK__PAUSE_STATE__H
