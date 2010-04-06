#include "character.h"
#include "defines.h"
#include "special.h"

Character::Character(void)
:Drawable(), direction(RIGHT), animdir(1), v_speed(0), h_speed(0), col_width(0.0),
col_height(0.0), col_x_offset(0.0), col_y_offset(0.0) {}

Character::Character(float x, float y, int num, int frames, Texture *tex,
                     direc dir, int vs, int hs, FMOD_SYSTEM *sys,
                     FMOD_SOUND *mu, FMOD_CHANNEL *mch)
:Drawable(x, y, num, frames, TILE, tex), direction(dir), animdir(1),
anim_delta(0), v_speed(vs), h_speed(hs), system(sys), music(mu),
music_channel(mch)
{
  col_width = get_width() * COLLISION_SPACE;
  col_height = get_height() * COLLISION_SPACE;
  
  col_x_offset = (get_width() - col_width) / 2.0;
  col_y_offset = (get_height() - col_height) / 2.0;
  
  /* play a sound and then immediately pause it
   * so that all we have to do later is call
   * pause_sound()
   */
  play_sound();
  pause_sound();
}

void Character::change_direction(direc dir)
{
  direction = dir;
  ((Drawable *)this)->change_direction(dir);
}

// moves the player and also changes the direction
// in which he moves
void Character::move(float x, float y)
{
  ((Drawable *)this)->move(x, y);
  if (x > 0)
  {
    change_direction(RIGHT);
  }
  else if (x < 0)
  {
    change_direction(LEFT);
  }
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
 * on a platform tile. Vertical speed
 * needs to be put into consideration
 */
bool Character::will_collide_platform(Map *m)
{
  return getVSpeed() > 0 && will_collide_tile(m, PLATFORM, NULL);
}

/* checks collision with the map
 * on a tile which is of type
 * "tile"
 */
bool Character::will_collide_tile(Map *m, tile_type tile, int coordinates[2])
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

bool Character::will_collide_specials(vector<Special *>specials, int cur,
                                      int *collide)
{
  if (getVSpeed() <= 0)
  {
    return false;
  }
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if (cur == (int)i)
    {
      continue;
    }
    if (will_collide(specials.at(i)))
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

bool Character::will_collide_screen_x(void)
{
  float char_x, char_y;
  
  get_top_left(char_x, char_y);
  
  if((getHSpeed() > 0 && char_x + get_width() > SCREEN_WIDTH - BORDER) ||
     (getHSpeed() < 0 && char_x < BORDER))
  {
    return true;
  }
  return false;
}

bool Character::will_collide_screen_y(void)
{
  float char_x, char_y;
  
  get_top_left(char_x, char_y);
  
  if((getVSpeed() > 0 && char_y + get_height() > SCREEN_HEIGHT - BORDER) ||
     (getVSpeed() < 0 && char_y < BORDER))
  {
    return true;
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

void Character::set_volume(float volume)
{
  FMOD_Channel_SetVolume(music_channel, volume);
}

// cleans up all garbage data
void Character::clean(void)
{
  // clean up the sound data
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
}
