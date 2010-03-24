#include "math.h"
#include "game_state.h"

Game_State::Game_State(void)
:p(NULL), m(NULL), gravity(true), collision(true) {}

Game_State::Game_State(Player *pl, Map *map, vector<Drawable *> mvs,
                       vector<Special *> sps, FMOD_SYSTEM *system)
:State(system), p(pl), m(map), moveables(mvs), specials(sps), gravity(true),
collision(true) {}

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
  p->get_top_left(x, y);
  m->get_top_left(mx, my);
  
  bool p_movex = true, m_movex = false;
  bool p_movey = true, m_movey = false;
  
  // if we're not colliding with anything,
  // then FALL MWAHAHA - written on very
  // little sleep
  if (gravity)
  {
    float y_val = y/TILE_HEIGHT;
    float my_val = my/TILE_HEIGHT;
    if (y_val - my_val < m->get_height())
    {
      p->setVSpeed(GRAVITY_SPEED);
      /*for (unsigned int i = 0; i < specials.size(); ++i)
      {
        specials.at(i)->setVSpeed(GRAVITY_SPEED);
      }*/
    }
  }
  
  if (collision)
  {
    // if we are colliding with ladder, then turn off gravity
    if (gravity)
    {
      gravity = !p->will_collide_ladder(m);
      if (!gravity)
      {
        p->setVSpeed(0);
      }
    }
    else
    {
      gravity = !p->will_collide_ladder(m);
    }
    
    // then check for collision
    p_movex = !p->will_collide_x(m);
    
    /* this is to deal with the case where the player has climbed the ladder
     * and then falls off. While both PLAYER_SPEED and GRAVITY_SPEED can
     * align properly by themselves, when used together, they can make it so
     * the player seems to "hover" above the floor. The following fixes this
     * hover issue.
     */
    if (!(p_movey = !p->will_collide_y(m) && !p->will_collide_platform(m)) &&
        p->getVSpeed() == GRAVITY_SPEED)
    {
      p->setVSpeed(PLAYER_SPEED);
      p_movey = !p->will_collide_y(m) && !p->will_collide_platform(m);
    }
  }
  
  // mxs and mys and the map movement speed in x and y
  float mxs = p->getHSpeed(), mys = p->getVSpeed();
  
  // if we collide with the map movement boundary
  // then freeze the player and move the map
  if (p_movex &&
      ((p->getHSpeed() > 0 && x + p->get_width() > SCREEN_WIDTH - BORDER) ||
      (p->getHSpeed() < 0 && x < BORDER)))
  {
    p_movex = false;
    m_movex = true;
    mxs = -mxs;
  }
  if (p_movey &&
      ((p->getVSpeed() > 0 && y + p->get_height() > SCREEN_HEIGHT - BORDER) ||
      (p->getVSpeed() < 0 && y < BORDER)))
  {
    p_movey = false;
    m_movey = true;
    mys = -mys;
  }
  
  if (m_movey && 0.0 >= my + mys &&
           my + mys + m->get_height() * TILE_HEIGHT >= SCREEN_HEIGHT)
  {
    m->move(0, mys);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      ((Drawable*)specials.at(i))->move(0, mys);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(0, mys);
    }
  }
  else if (p_movey)
  {
    p->move(0, p->getVSpeed());
  }
  
  if (m_movex && 0.0 >= mx + mxs &&
           mx + mxs + m->get_width() * TILE_WIDTH >= SCREEN_WIDTH)
  {
    m->move(mxs, 0);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      ((Drawable*)specials.at(i))->move(mxs, 0);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(mxs, 0);
    }
  }
  else if (p_movex)
  {
    p->move(p->getHSpeed(), 0);
  }
  
  // specials stuff
  float dx, dy, dist;
  if (collision)
  {
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (gravity)
      {
        if (!(specials.at(i)->will_collide_y(m)))
        {
          specials.at(i)->move(0, specials.at(i)->getVSpeed());
        }
      }
    }
  }
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    dx = (specials.at(i)->get_x()-p->get_x());
    dy = (specials.at(i)->get_y()-p->get_y());
    dist = sqrt((dx*dx)+(dy*dy));
    
    if (specials.at(i)->is_following() && dist > TOO_CLOSE)
    {
      if (specials.at(i)->get_mute())
      {
        specials.at(i)->set_mute(false);
      }
      specials.at(i)->setHSpeed(dx<0?PLAYER_SPEED:-PLAYER_SPEED);
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
    else if (dist < FOLLOW_DIST)
    {
      specials.at(i)->start_following(p);
    }
  }
  
  state_update();
}

void Game_State::key_pressed(unsigned char key, int x, int y)
{
  switch (key)
  {
    if (!gravity)
    {
      case 'w':
        p->setVSpeed(-PLAYER_SPEED);
        break;
      case 's':
        p->setVSpeed(PLAYER_SPEED);
        break;
    }
    case 'a':
      p->setHSpeed(-PLAYER_SPEED);
      break;
    case 'd':
      p->setHSpeed(PLAYER_SPEED);
      break;
  }
}

void Game_State::key_released(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 'w':
      p->setVSpeed(0);
      break;
    case 'a':
      p->setHSpeed(0);
      break;
    case 's':
      p->setVSpeed(0);
      break;
    case 'd':
      p->setHSpeed(0);
      break;
    // below is for debugging only
    // remove for final game version
    case 'g':
      gravity = !gravity;
      break;
    case 'h':
      collision = !collision;
      gravity = false;
      p->setVSpeed(0);
      break;
  }
}

void Game_State::clean(void)
{
  p->clean();
  state_clean();
}
