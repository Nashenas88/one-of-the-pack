#include "main_menu_state.h"

Main_Menu_State::Main_Menu_State(void)
: State() {}

Main_Menu_State::Main_Menu_State(Drawable *b, Drawable *p, FMOD_SYSTEM *s,
                                 FMOD_SOUND *mu, FMOD_CHANNEL *mch)
:State(s), background(b), pointer(p), selected_x(0), selected_y(0), music(mu),
channel(mch)
{
  pointer->move(-pointer->get_x() + MAIN_POINTER_START_X,
                -pointer->get_y() + MAIN_POINTER_START_Y);
}

void Main_Menu_State::draw(void)
{
  background->draw();
  pointer->draw();
}

void Main_Menu_State::update(int &delta)
{
  state_update();
}

void Main_Menu_State::key_pressed(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 'w':
      if(selected_y == 0)
      {
        selected_y = MAIN_MENU_ITEMS_Y - 1;
        pointer->move(0, MAIN_MENU_POINTER_MOVE_Y * (MAIN_MENU_ITEMS_Y - 1));
      }
      else
      {
        --selected_y;
        pointer->move(0, -MAIN_MENU_POINTER_MOVE_Y);
      }
      break;
    case 's':
      selected_y = (++selected_y) % MAIN_MENU_ITEMS_Y;
      if (selected_y == 0)
      {
        pointer->move(0, -MAIN_MENU_POINTER_MOVE_Y * (MAIN_MENU_ITEMS_Y - 1));
      }
      else
      {
        pointer->move(0, MAIN_MENU_POINTER_MOVE_Y);
      }
      break;
    case 'a':
      if(selected_x == 0)
      {
        selected_x = MAIN_MENU_ITEMS_X - 1;
        pointer->move(MAIN_MENU_POINTER_MOVE_X * (MAIN_MENU_ITEMS_X - 1), 0);
      }
      else
      {
        --selected_x;
        pointer->move(-MAIN_MENU_POINTER_MOVE_X, 0);
      }
      break;
    case 'd':
      selected_x = (++selected_x) % MAIN_MENU_ITEMS_X;
      if (selected_x == 0)
      {
        pointer->move(-MAIN_MENU_POINTER_MOVE_X * (MAIN_MENU_ITEMS_X - 1), 0);
      }
      else
      {
        pointer->move(MAIN_MENU_POINTER_MOVE_X, 0);
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

unsigned int Main_Menu_State::get_selected(void)
{
  return selected_y + MAIN_MENU_ITEMS_Y * selected_x + 1;
}

// plays the characters single sound
// (might possibly be renamed)
void Main_Menu_State::play_sound(void)
{
  if (music == NULL)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(get_system(), FMOD_CHANNEL_FREE, music,
                                 0, &channel);
  ERRCHECK(result);
}

// pauses (and unpauses) the characters single sound
// (might possibly be renamed)
void Main_Menu_State::pause_sound(void)
{
  if (music == NULL || channel == NULL)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  result = FMOD_Channel_GetPaused(channel, &sound_paused);
  ERRCHECK(result);
  
  sound_paused = !sound_paused;
  
  result = FMOD_Channel_SetPaused(channel, sound_paused);
  ERRCHECK(result);
}

void Main_Menu_State::clean(void)
{
  background->clean();
  pointer->clean();
  delete background;
  delete pointer;
  
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
}
