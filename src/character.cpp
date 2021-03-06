#include "character.h"
#include "defines.h"
#include "special.h"

Character::Character(void)
:Drawable(), direction(RIGHT), v_speed(0), h_speed(0), col_width(0.0),
col_height(0.0), col_x_offset(0.0), col_y_offset(0.0) {}

Character::Character(float x, float y, int num, int frames, Texture *tex,
                     direc dir, int vs, int hs, FMOD_SYSTEM *sys,
                     FMOD_SOUND *mu, FMOD_CHANNEL *mch)
:Drawable(x, y, num, frames, TILE, tex), anim_delta(0), v_speed(vs),
h_speed(hs), system(sys), music(mu), music_channel(mch), jump_height(0),
bounce(false)
{
  
  change_direction(dir);
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

Character::~Character(void)
{
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
  
  free(music_channel);
  
  delete (Drawable *) this;
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
bool Character::will_collide_Dx(Drawable *d)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  left_x1 += getHSpeed() + 2 * col_x_offset;
  top_y1 += col_y_offset;
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + col_width - 2*col_x_offset;
  right_x2 = left_x2 + d->get_width();
  bottom_y1 = top_y1 + col_height - 2*col_y_offset;
  bottom_y2 = top_y2 + d->get_height();
  
  if (bottom_y1 < top_y2) return false;
  if (top_y1 > bottom_y2) return false;
  
  if (right_x1 < left_x2) return false;
  if (left_x1 > right_x2) return false;
  
  return true;
}

bool Character::will_collide_Dy(Drawable *d, bool top)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  left_x1 += col_x_offset;
  top_y1 += getVSpeed() + col_y_offset;
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + col_width - 2*col_x_offset;
  right_x2 = left_x2 + d->get_width();
  bottom_y1 = top_y1 + col_height - 2*col_y_offset;
  bottom_y2 = top_y2 + d->get_height();
  
  if (top)
  {
    top_y1 = bottom_y1 - PLAYER_SPEED;
    bottom_y2 = top_y2 + PLAYER_SPEED;
  }
  
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
  if (!(getVSpeed() > 0))
  {
    return false;
  }
  int lx, rx, ty, by;
  
  float lxo, lxm;
  float rxo;
  float tyo, tym;
  float byo;
  
  get_top_left(lxo, tyo);
  lxo += getHSpeed() + col_x_offset;
  tyo += getVSpeed() + TILE_HEIGHT - 2 * col_y_offset - PLAYER_SPEED;
  rxo = lxo + col_width - col_x_offset;
  byo = tyo + 10;//col_height - col_y_offset;
  
  m->get_top_left(lxm, tym);
  
  lx = (int)((lxo - lxm) / TILE_WIDTH);
  rx = (int)((rxo - lxm - col_x_offset) / TILE_WIDTH);
  ty = (int)((tyo - tym) / TILE_HEIGHT);
  by = (int)((byo - tym - col_y_offset) / TILE_HEIGHT);
  
  for (int i = lx; i <= rx; ++i)
  {
    for (int j = ty; j <= by; ++j)
    {
      if(m->get_tile(i,j) == PLATFORM1 ||
         m->get_tile(i,j) == PLATFORM2 ||
         m->get_tile(i,j) == PLATFORM3 ||
         m->get_tile(i,j) == PLATFORM4 ||
         m->get_tile(i,j) == PLATFORM5)
      {
        if (j * TILE_HEIGHT + tym + PLAYER_SPEED >= tyo)
        {
          return true;
        }
      }
    }
  }
  
  return false;
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

bool Character::will_collide_specials_x(vector<Special *>specials, int cur,
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
    if (will_collide_Dx(specials.at(i)))
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

bool Character::will_collide_specials_y(vector<Special *>specials, int cur,
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
    if (will_collide_Dy(specials.at(i), true))
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

bool Character::will_collide_moveables_x(vector<Moveable *>moveables, int cur,
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

bool Character::will_collide_moveables_y(vector<Moveable *>moveables, int cur,
                                       int *collide)
{
  bool only_top = true;
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    if (cur == (int)i)
    {
      continue;
    }
    if (getVSpeed() <= 0)
    {
      only_top = false;
    }
    if (will_collide_Dy(moveables.at(i), only_top))
    {
      if (collide)
      {
        *collide = (int)i;
      }
      return true;
    }
    only_top = true;
  }
  return false;
}

bool Character::will_collide_rubber_x(Map *m)
{
  int temp_speed;
  bool answer;
  temp_speed = getVSpeed();
  setVSpeed(0);
  answer = will_collide_tile(m, RUBBER, NULL);
  setVSpeed(temp_speed);
  return answer;
}

bool Character::will_collide_rubber_y(Map *m)
{
  int temp_speed;
  bool answer;
  temp_speed = getHSpeed();
  setHSpeed(GRAVITY_SPEED);
  answer = will_collide_tile(m, RUBBER, NULL);
  setHSpeed(temp_speed);
  return answer;
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
  if (music == NULL)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, music,
                                 0, &music_channel);
  ERRCHECK(result);
}

// pauses (and unpauses) the characters single sound
// (might possibly be renamed)
void Character::pause_sound(void)
{
  if (music == NULL || music_channel == NULL)
  {
    return;
  }
  
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
