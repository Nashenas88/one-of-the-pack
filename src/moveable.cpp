#include <iostream>

#include "moveable.h"
#include "special.h"

Moveable::Moveable(void)
:Drawable(),gravity(false),rubber(false),v_speed(0),h_speed(0) {}

Moveable::Moveable(float x, float y, int start_x, int start_y, int num,
                   int frames, Texture *tex, bool g, bool r, Kurt *c)
:Drawable(x, y, num, frames, TILE, tex), gravity(g), rubber(r),
freezeable(g), v_speed(0), h_speed(0), creator(c)
{
  loc[0] = start_x;
  loc[1] = start_y;
}

void Moveable::reset(Map *m)
{
  float move_x, move_y;
  
  move_x = loc[0] * TILE_WIDTH + m->get_x() - get_x();
  move_y = loc[1] * TILE_HEIGHT + m->get_y() - get_y();
  move(move_x, move_y);
  for (unsigned int i = 0; i < links.size(); ++i)
  {
    links.at(i)->move(move_x, move_y);
  }
  setHSpeed(0, m);
  setVSpeed(0, m);
}

void Moveable::move(float x, float y)
{
  ((Drawable*)this)->move(x, y);
  if (creator)
  {
    ((Special*)creator)->setHSpeed((int)(x>0?x+0.55f:x-0.5f));
    ((Special*)creator)->setVSpeed((int)(y>0?y+0.5f:y-0.5f));
  }
}

bool Moveable::will_collide_Dx(Drawable *d)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  left_x1 += getHSpeed();
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + get_width() - 1;
  right_x2 = left_x2 + d->get_width() - 1;
  bottom_y1 = top_y1 + get_height() - 1;
  bottom_y2 = top_y2 + d->get_height() - 1;
  
  if (bottom_y1 < top_y2) return false;
  if (top_y1 > bottom_y2) return false;
  
  if (right_x1 < left_x2) return false;
  if (left_x1 > right_x2) return false;
  
  return true;
}

bool Moveable::will_collide_Dy(Drawable *d)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  top_y1 += getVSpeed();
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + get_width() - 1;
  right_x2 = left_x2 + d->get_width() - 1;
  bottom_y1 = top_y1 + get_height() - 1;
  bottom_y2 = top_y2 + d->get_height() - 1;
  
  if (bottom_y1 < top_y2) return false;
  if (top_y1 > bottom_y2) return false;
  
  if (right_x1 < left_x2) return false;
  if (left_x1 > right_x2) return false;
  
  return true;
}

/* checks collision with the map only
 * in the x-direction. For speed it
 * determines the (x,y) coordinates of
 * the character on the map and checks
 * collision with all blocks that surround
 * that (x,y) location
 */
bool Moveable::will_collide_x(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed();
  rxo = lxo + TILE_WIDTH - 1;
  byo = tyo + TILE_HEIGHT - 1;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym) / TILE_HEIGHT);
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_blocker(i,j))
      {
        return true;
      }
    }
  }
  
  return false;
}

/* checks collision with the map only
 * in the y-direction. For speed it
 * determines the (x,y) coordinates of
 * the character on the map and checks
 * collision with all blocks that surround
 * that (x,y) location
 */
bool Moveable::will_collide_y(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  tyo += getVSpeed();
  rxo = lxo + TILE_WIDTH - 1;
  byo = tyo + TILE_HEIGHT - 1;
  
  m->get_top_left(lxm, tym);
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym) / TILE_HEIGHT);
  
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_blocker(i,j))
      {
        return true;
      }
    }
  }
  
  return false;
}

bool Moveable::will_collide_moveables_x(vector<Moveable *>moveables, int cur,
                                       int *collide)
{
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    if (group_num == moveables.at(i)->get_group_num())
    {
      continue;
    }
    
    if (will_collide_Dx(moveables.at(i)))
    {
      if (collide)
      {
        *collide = (int)i;
      }
      return true;
    }
  }
  return false;
}

bool Moveable::will_collide_moveables_y(vector<Moveable *>moveables, int cur,
                                       int *collide)
{
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    if (group_num == moveables.at(i)->get_group_num())
    {
      continue;
    }
    
    if (will_collide_Dy(moveables.at(i)))
    {
      if (collide)
      {
        *collide = (int)i;
      }
      return true;
    }
  }
  return false;
}

bool Moveable::will_collide_specials_x(vector<Special *>specials, int *collide)
{
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if (specials.at(i)->get_type() == KURT &&
        (Kurt*)specials.at(i) == creator)
    {
      continue;
    }
    if (specials.at(i)->will_collide_Dx(this))
    {
      if (collide)
      {
        *collide = i;
      }
      return true;
    }
  }
  return false;
}

bool Moveable::will_collide_specials_y(vector<Special *>specials, int *collide)
{
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if (specials.at(i)->get_type() == KURT &&
        (Kurt*)specials.at(i) == creator)
    {
      continue;
    }
    if (specials.at(i)->will_collide_Dy(this,false))
    {
      if (collide)
      {
        *collide = i;
      }
      return true;
    }
  }
  return false;
}

bool Moveable::will_collide_tile(Map *m, tile_type tile, int coordinates[2])
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed();
  tyo += getVSpeed();
  rxo = lxo + TILE_WIDTH;
  byo = tyo + TILE_HEIGHT;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym) / TILE_HEIGHT);
  
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_tile(i,j) == tile)
      {
        if (coordinates != NULL)
        {
          coordinates[0] = i;
          coordinates[1] = j;
        }
        return true;
      }
    }
  }
  
  return false;
}

void Moveable::setHSpeed(int hs, Map *m)
{
  h_speed = hs;
  if (creator)
  {
    ((Special*)creator)->setHSpeed(hs);
  }
  for (unsigned int i = 0; i < links.size(); ++i)
  {
    links.at(i)->setHSpeed(hs);
  }
}

void Moveable::setVSpeed(int vs, Map *m)
{
  v_speed = vs;
  if (creator)
  {
    ((Special*)creator)->setVSpeed(vs);
  }
  for (unsigned int i = 0; i < links.size(); ++i)
  {
    links.at(i)->setVSpeed(vs);
  }
}

void Moveable::set_gravity(bool g)
{
  gravity = g;
  for (unsigned int i = 0; i < links.size(); ++i)
  {
    links.at(i)->set_grav(g);
  }
}
