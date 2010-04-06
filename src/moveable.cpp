#include "moveable.h"

Moveable::Moveable(void)
:Drawable(),gravity(false),v_speed(0),h_speed(0) {}

Moveable::Moveable(float x, float y, int num, int frames, Texture *tex,
                   bool g)
:Drawable(x, y, num, frames, TILE, tex), gravity(g), v_speed(0), h_speed(0) {}

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
      if(m->get_blocker(i,j))
      {
        return true;
      }
    }
  }
  
  return false;
}