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
  Game_State *game_state;
  Drawable *background;
  Drawable *map;
  Drawable *pointer;
  Drawable *movers;
  Drawable *statics;
  Drawable *paused_names;
  unsigned int selected;
  unsigned int anim_timer;
  
public:
  Pause_State(void);
  Pause_State(FMOD_SYSTEM *system, Game_State *gs, Drawable *b, Drawable *m,
              Drawable *p, Texture *icons, Drawable *pn);
  
  void draw(void);
  
  void update(int &delta);
  
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  void pause_sounds(void);
  
  unsigned int get_selected(void) {return selected;}
  void reset_selected(void);
  
  void clean(void);
};
#endif // FLOCK__PAUSE_STATE__H
