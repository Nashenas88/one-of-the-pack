#ifndef FLOCK__TUTORIAL_STATE__H
#define FLOCK__TUTORIAL_STATE__H

#include <vector>
using namespace std;

#include "drawable.h"
#include "FMOD_includes.h"
#include "state.h"

class Tutorial_State : public State
{
private:
  vector<Drawable *> slides;
  unsigned int current_slide;
  
  FMOD_SOUND *music;
  FMOD_CHANNEL *channel;
  
  FMOD_BOOL sound_paused;
  
public:
  Tutorial_State(void);
  Tutorial_State(vector<Drawable *> images, FMOD_SYSTEM *s, FMOD_SOUND *mu,
                 FMOD_CHANNEL *mch);
  
  void draw(void);
  
  void update(int &delta);
  
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  void play_sound(void);
  void pause_sound(void);
  
  void clean(void);
};
#endif // FLOCK__MAIN_MENU_STATE__H
