#ifndef FLOCK__MAIN_MENU_STATE__H
#define FLOCK__MAIN_MENU_STATE__H

#include "drawable.h"
#include "FMOD_includes.h"
#include "state.h"

class Main_Menu_State : public State
{
private:
  Drawable *background;
  Drawable *pointer;
  unsigned int selected_x, selected_y;
  
  FMOD_SOUND *music;
  FMOD_CHANNEL *channel;
  
  FMOD_BOOL sound_paused;
  
public:
  Main_Menu_State(void);
  Main_Menu_State(Drawable *b, Drawable *p, FMOD_SYSTEM *s, FMOD_SOUND *mu);
  ~Main_Menu_State(void);
  
  void draw(void);
  
  void update(int &delta);
  
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  unsigned int get_selected(void);
  
  void play_sound(void);
  void pause_sound(void);
};
#endif // FLOCK__MAIN_MENU_STATE__H
