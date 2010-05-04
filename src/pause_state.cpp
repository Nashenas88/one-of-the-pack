#include "pause_state.h"

Pause_State::Pause_State(void)
: State() {}

Pause_State::Pause_State(FMOD_SYSTEM *system, Game_State *gs, Drawable *b,
                         Drawable *m, Drawable *p, Texture *icons, Drawable *pn)
:State(system), game_state(gs), background(b), map(m), pointer(p),
paused_names(pn), selected(0), anim_timer(0)
{
  movers = new Drawable (0.0f, 0.0f, 1, 1, MAP_ICON, icons);
  statics = new Drawable (0.0f, 0.0f, 1, 1, MAP_ICON, icons);
}

void Pause_State::draw(void)
{
  Special *spec;
  Moveable *move;
  Map *m;
  int x, y;
  
  game_state->draw();
  background->draw();
  map->draw();
  
  for (unsigned int i = 0; i < ENGINEER_ICON; ++i)
  {
    statics->set_tex_num(i+1);
    statics->move(-statics->get_x() + PAUSE_MAP_X + 256,
                  -statics->get_y() + PAUSE_MAP_Y + 16 * i);
    statics->draw();
  }
  paused_names->move(-paused_names->get_x() + PAUSE_MAP_X + 266,
                     -paused_names->get_y() + PAUSE_MAP_Y + 1);
  paused_names->draw();
  
  m = game_state->get_map();
  for (int i = 0; i < m->get_width(); ++i)
  {
    for (int j = 0; j < m->get_height(); ++j)
    {
      switch(m->get_tile(i, j))
      {
        case BREAKABLE1:
        case BREAKABLE2:
        case BREAKABLE3:
          statics->set_tex_num(BREAK_ICON);
          statics->move(-statics->get_x() + PAUSE_MAP_X + (i - 1) * 2,
                        -statics->get_y() + PAUSE_MAP_Y + (j - 1) * 2);
          statics->draw();
          break;
        case GOAL:
          statics->set_tex_num(GOAL_ICON);
          statics->move(-statics->get_x() + PAUSE_MAP_X + (i - 1) * 2,
                        -statics->get_y() + PAUSE_MAP_Y + (j - 1) * 2);
          statics->draw();
          break;
        default:
          break;
      }
    }
  }
  
  for (unsigned int i = 0; i < game_state->get_moveables().size(); ++i)
  {
    move = game_state->get_moveables().at(i);
    if (move->get_gravity())
    {
      statics->set_tex_num(PUSH_ICON);
    }
    else
    {
      statics->set_tex_num(HOVER_ICON);
    }
    statics->move(-statics->get_x() + PAUSE_MAP_X, -statics->get_y() + PAUSE_MAP_Y);
    m->calculate_location(move, x, y);
    statics->move((x - 1) * 2, (y - 1) * 2);
    statics->draw();
  }
  for (unsigned int i = game_state->get_specials().size() - 1;
       i < game_state->get_specials().size(); --i)
  {
    spec = game_state->get_specials().at(i);
    switch (spec->get_type())
    {
      case AHNOLD:
        movers->set_tex_num(AHNOLD_ICON);
        break;
      case JUMPER:
        movers->set_tex_num(JUMPER_ICON);
        break;
      case PARIS:
        movers->set_tex_num(PARIS_ICON);
        break;
      case KURT:
        movers->set_tex_num(KURT_ICON);
        break;
      case ENGINEER:
        movers->set_tex_num(ENGINEER_ICON);
        break;
      default:
        break;
    }
    movers->move(-movers->get_x() + PAUSE_MAP_X, -movers->get_y() + PAUSE_MAP_Y);
    m->calculate_location(spec, x, y);
    movers->move((x - 1) * 2, (y - 1) * 2);
    movers->draw();
  }
  movers->set_tex_num(PLAYER_ICON);
  movers->move(-movers->get_x() + PAUSE_MAP_X, -movers->get_y() + PAUSE_MAP_Y);
  m->calculate_location(game_state->get_player(), x, y);
  movers->move((x - 1) * 2, (y - 1) * 2);
  movers->draw();
  
  pointer->draw();
}

void Pause_State::update(int &delta)
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

void Pause_State::key_pressed(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 'w':
      if(selected == 0)
      {
        selected = MENU_ITEMS - 1;
        pointer->move(0, POINTER_MOVE * 2);
      }
      else
      {
        --selected;
        pointer->move(0, -POINTER_MOVE);
      }
      break;
    case 's':
      selected = (++selected) % MENU_ITEMS;
      if (selected == 0)
      {
        pointer->move(0, -POINTER_MOVE * 2);
      }
      else
      {
        pointer->move(0, POINTER_MOVE);
      }
      break;
  }
}

void Pause_State::key_released(unsigned char key, int x, int y) {}

void Pause_State::special_pressed(int key, int x, int y)
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

void Pause_State::special_released(int key, int x, int y) {}

void Pause_State::reset_selected(void)
{
  while (selected != 0)
  {
    key_pressed('w', 0, 0);
  }
}

void Pause_State::clean(void)
{
  background->clean();
  map->clean();
  pointer->clean();
  delete background;
  delete map;
  delete pointer;
  delete movers;
  delete statics;
  paused_names->clean();
  delete paused_names;
  game_state->clean();
  delete game_state;
}
