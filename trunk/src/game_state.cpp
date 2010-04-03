#include "math.h"
#include "game_state.h"

Game_State::Game_State(void)
:p(NULL), c(NULL), m(NULL), gravity(true), collision(true) {}

Game_State::Game_State(Player *pl, Map *map, vector<Drawable *> mvs,
                       vector<Special *> sps, FMOD_SYSTEM *system)
:State(system), p(pl), c(pl), m(map), moveables(mvs), specials(sps),
next_special(0), gravity(true), collision(true) {}

// draw static background, then map, then specials, then moveables, then player
void Game_State::draw(void)
{
  m->get_background()->draw();
  m->draw();
  for (unsigned int i = 0; i < specials.size(); i++)
  {
    specials.at(i)->draw();
  }
  for (unsigned int i = 0; i < moveables.size(); i++)
  {
    moveables.at(i)->draw();
  }
  p->draw();
}

// this is called every time the scene
// needs to be updated
void Game_State::update(int &delta)
{
  float x, y, mx, my;
  c->get_top_left(x, y);
  m->get_top_left(mx, my);
  
  bool c_movex = true, m_movex = false;
  bool c_movey = true, m_movey = false;
  bool p_movey = true;
  bool animate = false;
  
  // if we're not colliding with anything,
  // then FALL MWAHAHA - written on very
  // little sleep
  if (gravity)
  {
    float y_val = y/TILE_HEIGHT;
    float my_val = my/TILE_HEIGHT;
    if (y_val - my_val < m->get_height())
    {
      c->setVSpeed(GRAVITY_SPEED);
      /*for (unsigned int i = 0; i < specials.size(); ++i)
      {
        specials.at(i)->setVSpeed(GRAVITY_SPEED);
      }*/
    }
    float px, py;
    p->get_top_left(px, py);
    float py_val = py / TILE_HEIGHT;
    if (py_val - my_val < m->get_height())
    {
      p->setVSpeed(GRAVITY_SPEED);
    }
  }
  
  if (collision)
  {
    // if player collides with the goal, no need to
    // do anything else since we're done already
    if (p->will_collide_tile(m, GOAL))
    {
      delta = -1;
      return;
    }
    
    // if we are colliding with ladder, then turn off gravity
    if (gravity)
    {
      gravity = !p->will_collide_tile(m, LADDER);
      if (!gravity)
      {
        p->setVSpeed(0);
      }
    }
    else if (c == p)
    {
      gravity = !p->will_collide_tile(m, LADDER);
    }
    else
    {
      gravity = true;
    }
    
    // then check for collision
    c_movex = !c->will_collide_x(m);
    
    /* this is to deal with the case where the player has climbed the ladder
     * and then falls off. While both PLAYER_SPEED and GRAVITY_SPEED can
     * align properly by themselves, when used together, they can make it so
     * the player seems to "hover" above the floor. The following fixes this
     * hover issue.
     */
    c_movey = !c->will_collide_y(m) && !c->will_collide_platform(m);
    if (c != p)
    {
      p_movey = !p->will_collide_y(m) && !p->will_collide_platform(m);
      c_movey = c_movey && !c->will_collide_tile(m, LADDER);
    }
    if (c == p && !c_movey && c->getVSpeed() == GRAVITY_SPEED)
    {
      c->setVSpeed(PLAYER_SPEED);
      c_movey = !c->will_collide_y(m) && !c->will_collide_platform(m);
      if (c != p)
      {
        c_movey = c_movey && !c->will_collide_tile(m, LADDER);
      }
    }
    if (c != p && !p_movey && p->getVSpeed() == GRAVITY_SPEED)
    {
      p->setVSpeed(PLAYER_SPEED);
      p_movey = !p->will_collide_y(m) && !p->will_collide_platform(m);
    }
  }
  
  // mxs and mys and the map movement speed in x and y
  float mxs = c->getHSpeed(), mys = c->getVSpeed();
  
  // if we collide with the map movement boundary
  // then freeze the player and move the map
  if (c_movex &&
      ((c->getHSpeed() > 0 && x + c->get_width() > SCREEN_WIDTH - BORDER) ||
      (c->getHSpeed() < 0 && x < BORDER)))
  {
    c_movex = false;
    m_movex = true;
    mxs = -mxs;
  }
  if (c_movey &&
      ((c->getVSpeed() > 0 && y + c->get_height() > SCREEN_HEIGHT - BORDER) ||
      (c->getVSpeed() < 0 && y < BORDER)))
  {
    c_movey = false;
    m_movey = true;
    mys = -mys;
  }
  
  if (m_movey && 0.0 >= my + mys &&
           my + mys + m->get_height() * TILE_HEIGHT >= SCREEN_HEIGHT)
  {
    m->move(0, mys);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      if (c == specials.at(i))
      {
        ((Drawable*)p)->move(0, mys);
        continue;
      }
      ((Drawable*)specials.at(i))->move(0, mys);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(0, mys);
    }
  }
  else if (c_movey)
  {
    c->move(0, c->getVSpeed());
  }
  
  if (c != p && p_movey)
  {
    p->move(0, p->getVSpeed());
  }
  
  if (m_movex && 0.0 >= mx + mxs &&
      mx + mxs + m->get_width() * TILE_WIDTH >= SCREEN_WIDTH)
  {
    animate = true;
    m->move(mxs, 0);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      if (c == specials.at(i))
      {
        ((Drawable*)p)->move(mxs, 0);
        continue;
      }
      ((Drawable*)specials.at(i))->move(mxs, 0);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(mxs, 0);
    }
  }
  else if (c_movex && c->getHSpeed() != 0)
  {
    animate = true;
    c->move(c->getHSpeed(), 0);
  }
  
  // player animation
  if (animate && c->get_num_frames() > 1)
  {
    if (delta > DELTA_DELAY)
    {
      delta = 0;
      if ((c->get_animdir() == 1 &&
          c->get_cur_frame() == c->get_num_frames()) ||
          (c->get_animdir() == -1 && c->get_cur_frame() == 1))
      {
        c->set_animdir(c->get_animdir() * -1);
      }
      c->set_cur_frame(c->get_cur_frame() + c->get_animdir());
    }
    else
    {
      ++delta;
    }
  }
  else
  {
    delta = 0;
    c->set_cur_frame(1);
  }
  
  // specials stuff
  float dx, dy, dist;
  unsigned int cur_frame, a_delta;
  Special *s;
  if (collision)
  {
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if(c == specials.at(i))
      {
        continue;
      }
      if (gravity)
      {
        if (!(specials.at(i)->will_collide_y(m) ||
              specials.at(i)->will_collide_tile(m, LADDER) ||
              specials.at(i)->will_collide_platform(m)))
        {
          specials.at(i)->move(0, specials.at(i)->getVSpeed());
        }
      }
    }
  }
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if (c == specials.at(i))
    {
      continue;
    }
    
    a_delta = specials.at(i)->get_delta();
    if (specials.at(i)->get_tex_num() == ABILITY)
    {
      if (a_delta > SPECIAL_DELTA_DELAY)
      {
        specials.at(i)->set_delta(0);
        cur_frame = specials.at(i)->get_cur_frame();
        if (cur_frame < specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(cur_frame + 1);
        }
        else if (cur_frame == specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(1);
          specials.at(i)->set_tex_num(SPECIAL);
        }
      }
      else
      {
        specials.at(i)->set_delta(++a_delta);
      }
    }
    
    dx = (specials.at(i)->get_x() - p->get_x());
    dy = (specials.at(i)->get_y() - p->get_y());
    dist = sqrt((dx*dx)+(dy*dy));
    
    if (specials.at(i)->is_following() && dist > TOO_CLOSE)
    {
      if (specials.at(i)->get_mute())
      {
        specials.at(i)->set_mute(false);
      }
      specials.at(i)->setHSpeed(!dx?0:(dx<0?PLAYER_SPEED:-PLAYER_SPEED));
      if (collision)
      {
        if (!(specials.at(i)->will_collide_x(m)))
        {
          specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
        }
      }
      else
      {
        specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
      }
    }
    else if (dist < FOLLOW_DIST && !specials.at(i)->is_following())
    {
      specials.at(i)->start_following();
      if (i > (unsigned int) next_special)
      {
        s = specials.at(i);
        specials.at(i) = specials.at(next_special);
        specials.at(next_special) = s;
        ++next_special;
      }
      else if (i == (unsigned int) next_special)
      {
        ++next_special;
      }
    }
  }
  
  state_update();
}

void Game_State::key_pressed(unsigned char key, int x, int y)
{
  if (key >= 'A' && key <= 'Z')
  {
    key += 'a' - 'A';
  }
  switch (key)
  {
    if (!gravity)
    {
      case 'w':
        c->setVSpeed(-PLAYER_SPEED);
        break;
      case 's':
        c->setVSpeed(PLAYER_SPEED);
        break;
    }
    case 'a':
      c->setHSpeed(-PLAYER_SPEED);
      break;
    case 'd':
      c->setHSpeed(PLAYER_SPEED);
      break;
    case 'p':
      for (unsigned int i = 0; i < next_special && i < specials.size(); ++i)
      {
        specials.at(i)->set_tex_num(ABILITY);
      }
      break;
    case 'q':
      if (c != p)
      {
        ((Special*)c)->stop_following();
        ((Special*)c)->set_mute(true);
      }
    case '0':
      c->setVSpeed(GRAVITY_SPEED);
      c = p;
      c->setVSpeed(0);
      c->setHSpeed(0);
      
      c->set_volume(MAX_VOLUME);
      for (unsigned int i = 0; i < specials.size(); ++i)
      {
        specials.at(i)->set_volume(MAX_VOLUME);
      }
      
      center();
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if ((unsigned int) (key - 49) < next_special &&
          (unsigned int) (key - 49) < specials.size())
      {
        c = specials.at(key - 49);
        c->setVSpeed(0);
        c->setHSpeed(0);
        
        p->set_volume(UNFOCUSED_VOLUME);
        for (unsigned int i = 0; i < specials.size(); ++i)
        {
          if (c == specials.at(i))
          {
            continue;
          }
          specials.at(i)->set_volume(UNFOCUSED_VOLUME);
        }
        
        center();
      }
      break;
  }
}

void Game_State::key_released(unsigned char key, int x, int y)
{
  if (key >= 'A' && key <= 'Z')
  {
    key += 'a' - 'A';
  }
  switch (key)
  {
    case 'w':
      c->setVSpeed(0);
      break;
    case 'a':
      c->setHSpeed(0);
      break;
    case 's':
      c->setVSpeed(0);
      break;
    case 'd':
      c->setHSpeed(0);
      break;
    // below is for debugging only
    // remove for final game version
    case 'h':
      collision = !collision;
      gravity = false;
      c->setVSpeed(0);
      p->setVSpeed(0);
      break;
  }
}

void Game_State::special_pressed(int key, int x, int y)
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

void Game_State::special_released(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      key_released('w', x, y);
      break;
    case GLUT_KEY_DOWN:
      key_released('s', x, y);
      break;
    case GLUT_KEY_RIGHT:
      key_released('d', x, y);
      break;
    case GLUT_KEY_LEFT:
      key_released('a', x, y);
      break;
  }
}

void Game_State::center(void)
{
  float x, y, mx, my, move_x, move_y, map_width, map_height;
  
  c->get_top_left(x, y);
  m->get_top_left(mx, my);
  move_x = SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f - x;
  move_y = SCREEN_HEIGHT / 2.0f - TILE_HEIGHT - y;
  map_width = m->get_width() * TILE_WIDTH;
  map_height = m->get_height() * TILE_HEIGHT;
  
  if (move_x + mx > 0)
  {
    move_x = -mx;
  }
  else if (move_x + mx - SCREEN_WIDTH < -map_width)
  {
    move_x = map_width + mx - SCREEN_WIDTH;
  }
  if (move_y + my > 0)
  {
    move_y = -my;
  }
  else if (move_y + my - SCREEN_HEIGHT < -map_height)
  {
    move_y = map_height + my - SCREEN_HEIGHT;
  }
  
  m->move(move_x,move_y);
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    ((Drawable *)specials.at(i))->move(move_x,move_y);
  }
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->move(move_x,move_y);
  }
  ((Drawable *)p)->move(move_x, move_y);
}

void Game_State::pause_volume(void)
{
  p->set_volume(PAUSE_VOLUME);
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->set_volume(PAUSE_VOLUME);
  }
}

void Game_State::unpause_volume(void)
{
  c->set_volume(MAX_VOLUME);
  if (c == p)
  {
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      specials.at(i)->set_volume(MAX_VOLUME);
    }
  }
  else
  {
    p->set_volume(UNFOCUSED_VOLUME);
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (c == specials.at(i))
      {
        continue;
      }
      specials.at(i)->set_volume(UNFOCUSED_VOLUME);
    }
  }
}

void Game_State::clean(void)
{
  p->clean();
  delete p;
  m->clean();
  delete m;
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->clean();
    delete moveables.at(i);
  }
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->clean();
    delete specials.at(i);
  }
  state_clean();
}
