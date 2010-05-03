#ifndef FLOCK__MAIN_MENU_STATE__H
#define FLOCK__MAIN_MENU_STATE__H

#include <vector>
using namespace std;

#include "drawable.h"
#include "FMOD_includes.h"
#include "state.h"
#include "game_state.h"

class Main_Menu_State : public State
{
private:
  Drawable *background;
  Drawable *pointer;
  unsigned int selected;
  
public:
  Main_Menu_State(void);
  Main_Menu_State(FMOD_SYSTEM *system, Drawable *b, Drawable *p);
  
  void draw(void);
  
  void update(int &delta);
  
  void key_pressed(unsigned char key, int x, int y);
  void key_released(unsigned char key, int x, int y);
  void special_pressed(int key, int x, int y);
  void special_released(int key, int x, int y);
  
  unsigned int get_selected(void) {return selected;}
  void reset_selected(void);
  
  void clean(void);
};
#endif // FLOCK__MAIN_MENU_STATE__H
