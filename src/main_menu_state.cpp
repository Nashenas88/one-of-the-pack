#include "main_menu_state.h"

Main_Menu_State::Main_Menu_State(void)
: State(), dspparameq(0) {}

Main_Menu_State::Main_Menu_State(FMOD_SYSTEM *system, FMOD_DSP *pe, Game_State *gs,
              Drawable *b, Drawable *m, Drawable *p, Texture *icons)
:State(system), dspparameq(pe), game_state(gs), background(b),
map(m), pointer(p), selected(0), anim_timer(0)
{
  movers = new Drawable (0.0f, 0.0f, 1, 1, MAP_ICON, icons);
  statics = new Drawable (0.0f, 0.0f, 1, 1, MAP_ICON, icons);
}

void Main_Menu_State::draw(void)
{
  
  
  pointer->draw();
}

void Main_Menu_State::update(int &delta)
{
  if (anim_timer == PAUSE_ANIM_DELAY)
  {
    anim_timer = 0;
    int current;
    current = movers->get_cur_frame();
    ++current;
    if (current > PAUSE_ICON_FRAMES)
    {
      current = 1;
    }
    movers->set_cur_frame(current);
  }
  else
  {
    ++anim_timer;
  }
  state_update();
}

void Main_Menu_State::key_pressed(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 'w':
      if(selected == 0)
      {
        selected = MENU_ITEMS - 1;
        pointer->move(0, MAIN_MENU_POINTER_MOVE * 2);
      }
      else
      {
        --selected;
        pointer->move(0, -MAIN_MENU_POINTER_MOVE);
      }
      break;
    case 's':
      selected = (++selected) % MENU_ITEMS;
      if (selected == 0)
      {
        pointer->move(0, -MAIN_MENU_POINTER_MOVE * 2);
      }
      else
      {
        pointer->move(0, MAIN_MENU_POINTER_MOVE);
      }
      break;
  }
}

void Main_Menu_State::key_released(unsigned char key, int x, int y) {}

void Main_Menu_State::special_pressed(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      key_pressed('w', x, y);
      break;
    case GLUT_KEY_DOWN:
      key_pressed('s', x, y);
      break;
    case GLUT_KEY_RIGHT:
      key_pressed('d', x, y);
      break;
    case GLUT_KEY_LEFT:
      key_pressed('a', x, y);
      break;
  }
}

void Main_Menu_State::special_released(int key, int x, int y) {}

void Main_Menu_State::reset_selected(void)
{
  while (selected != 0)
  {
    key_pressed('w', 0, 0);
  }
}

void Main_Menu_State::clean(void)
{
  background->clean();
  map->clean();
  pointer->clean();
  delete background;
  delete map;
  delete pointer;
  game_state->clean();
  delete game_state;
}
