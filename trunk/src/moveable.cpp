#include "moveable.h"

Moveable::Moveable(void)
:Drawable(),gravity(false),v_speed(0),h_speed(0) {}

Moveable::Moveable(float x, float y, int num, int frames, Texture *tex,
                   bool g)
:Drawable(x, y, num, frames, TILE, tex), gravity(g), v_speed(0), h_speed(0),
link(NULL) {}


bool Moveable::will_collide_Dx(Drawable *d)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  left_x1 += getHSpeed();
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + get_width();
  right_x2 = left_x2 + d->get_width();
  bottom_y1 = top_y1 + get_height();
  bottom_y2 = top_y2 + d->get_height();
  
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
  right_x1 = left_x1 + get_width();
  right_x2 = left_x2 + d->get_width();
  bottom_y1 = top_y1 + get_height();
  bottom_y2 = top_y2 + d->get_height();
  
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
    if (cur == (int)i)
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
    if (cur == (int)i)
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
    if (will_collide_Dx((Drawable *)specials.at(i)))
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
