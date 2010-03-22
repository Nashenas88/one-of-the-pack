#include "character.h"
#include "defines.h"

Character::Character(void)
:Drawable(), direction(DOWN), animdir(1), v_speed(0), h_speed(0), col_width(0.0),
col_height(0.0), col_x_offset(0.0), col_y_offset(0.0) {}

Character::Character(float x, float y, int num, int frames, Texture *tex,
                     direc dir, int vs, int hs, FMOD_SYSTEM *sys,
                     FMOD_SOUND *mu, FMOD_CHANNEL *mch)
:Drawable(x, y, num, frames, TILE, tex), direction(dir), animdir(1),
v_speed(vs), h_speed(hs), system(sys), music(mu), music_channel(mch)
{
  col_width = get_width() * 0.8f;
  col_height = get_height() * 0.8f;
  
  col_x_offset = (get_width() - col_width) / 2.0;
  col_y_offset = (get_height() - col_height) / 2.0;
  
  /* play a sound and then immediately pause it
   * so that all we have to do later is call
   * pause_sound()
   */
  play_sound();
  pause_sound();
}

// this is collision detection which checks
// collision between a character and a drawable
// currently it isn't being used
bool Character::will_collide(Drawable *d)
{
  float lx1, lx2;
  float rx1, rx2;
  float ty1, ty2;
  float by1, by2;
  
  get_top_left(lx1, ty1);
  lx1 += getHSpeed() + col_x_offset;
  ty1 += getVSpeed() + col_y_offset;
  d->get_top_left(lx2, ty2);
  rx1 = lx1 + col_width;
  rx2 = lx2 + d->get_width();
  by1 = ty1 + col_height;
  by2 = ty2 + d->get_height();
  
  if (by1 < ty2) return false;
  if (ty1 > by2) return false;
  
  if (rx1 < lx2) return false;
  if (lx1 > rx2) return false;
  
  return true;
}

/* checks collision with the map only
 * in the x-direction. For speed it
 * determines the (x,y) coordinates of
 * the character on the map and checks
 * collision with all blocks that surround
 * that (x,y) location
 */
bool Character::will_collide_x(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed() + col_x_offset;
  tyo += col_y_offset;
  rxo = lxo + col_width - col_x_offset;
  byo = tyo + col_height - col_y_offset;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm - col_x_offset) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym - col_y_offset) / TILE_HEIGHT);
  
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
bool Character::will_collide_y(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += col_x_offset;
  tyo += getVSpeed() + col_y_offset;
  rxo = lxo + col_width - col_x_offset;
  byo = tyo + col_height - col_y_offset;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm - col_x_offset) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym - col_y_offset) / TILE_HEIGHT);
  
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
 * on a ladder tile. For speed it
 * determines the (x,y) coordinates of
 * the character on the map and checks
 * collision with all blocks that surround
 * that (x,y) location
 */
bool Character::will_collide_ladder(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed() + col_x_offset;
  tyo += getVSpeed() + col_y_offset;
  rxo = lxo + col_width - col_x_offset;
  byo = tyo + col_height - col_y_offset;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm - col_x_offset) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym - col_y_offset) / TILE_HEIGHT);
  
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_tile(i,j) == LADDER)
      {
        return true;
      }
    }
  }
  
  return false;
}

/* checks collision with the map only
 * on a platform tile. For speed it
 * determines the (x,y) coordinates of
 * the character on the map and checks
 * collision with all blocks that surround
 * that (x,y) location
 */
bool Character::will_collide_platform(Map *m)
{
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed() + col_x_offset;
  tyo += getVSpeed() + col_y_offset;
  rxo = lxo + col_width - col_x_offset;
  byo = tyo + col_height - col_y_offset;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm - col_x_offset) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym - col_y_offset) / TILE_HEIGHT);
  
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_tile(i,j) == PLATFORM && getVSpeed() > 0)
      {
        return true;
      }
    }
  }
  
  return false;
}

// plays the characters single sound
// (might possibly be renamed)
void Character::play_sound(void)
{
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, music,
                                 0, &music_channel);
  ERRCHECK(result);
}

// pauses (and unpauses) the characters single sound
// (might possibly be renamed)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// play_sound MUST be called before this is ever called
void Character::pause_sound(void)
{
  FMOD_RESULT result;
  
  result = FMOD_Channel_GetPaused(music_channel, &sound_paused);
  ERRCHECK(result);
  
  sound_paused = !sound_paused;
  
  result = FMOD_Channel_SetPaused(music_channel, sound_paused);
  ERRCHECK(result);
}

// cleans up all garbage data
void Character::clean(void)
{
  // clean up the sound data
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
}
