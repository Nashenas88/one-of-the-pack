#include "pause_state.h"

Pause_State::Pause_State(void)
: State(), dspparameq(0) {}

Pause_State::Pause_State(FMOD_SYSTEM *system, FMOD_DSP *pe, Game_State *gs,
              vector<Drawable *> i, Drawable *b, Drawable *m, Drawable *p)
:State(system), dspparameq(pe), game_state(gs), items(i), background(b),
map(m), pointer(p), selected(0) {}

void Pause_State::draw(void)
{
  game_state->draw();
  background->draw();
  map->draw();
  /*for (unsigned int i = 0; i < items.size(); ++i)
  {
    items.at(i)->draw();
  }
  pointer->draw();*/
}

void Pause_State::update(int &delta)
{
  state_update();
}

void Pause_State::key_pressed(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 'w':
      if(selected == 0)
      {
        selected = items.size() - 1;
        pointer->move(0, items.at(0)->get_height() * (items.size() - 1));
      }
      else
      {
        --selected;
        pointer->move(0, -(items.at(0)->get_height()));
      }
      break;
    case 's':
      selected = (++selected) % items.size();
      if (selected == (items.size() - 1))
      {
        pointer->move(0, -(items.at(0)->get_height()) * (items.size() - 1));
      }
      else
      {
        pointer->move(0, items.at(0)->get_height());
      }
      break;
  }
}

void Pause_State::key_released(unsigned char key, int x, int y) {}

void Pause_State::clean(void)
{
  state_clean();
}
