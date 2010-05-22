
#include "special.h"
#include "defines.h"

Special::Special(void)
:Character()
{}

Special::Special(float x, float y, int map_x, int map_y, int num, int frames,
                 int abil_frames, Texture *tex, direc dir, int vs, int hs,
                 special_type t, FMOD_SYSTEM *sys, FMOD_SOUND *so,
                 FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac,
                 FMOD_SOUND *cs)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch),
following(true), controllable(false), num_abil_frames(abil_frames),
abil_sound(as), abil_channel(ac), collect_sound(cs), number(NULL), jump_delta(-1)
{
  loc[0] = map_x;
  loc[1] = map_y;
  type = t;
  set_mute(true);
}

Special::~Special(void)
{
  delete (Drawable *)this;
}

void Special::start_following(void)
{
  following = true;
  if (!controllable)
  {
    play_collect();
  }
  controllable = true;
  set_shadow(false);
}

void Special::stop_following(void)
{
  following = false;
  set_shadow(true);
}

void Special::play_effect(void)
{
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(get_system(), FMOD_CHANNEL_FREE, abil_sound,
                                 0, &abil_channel);
  ERRCHECK(result);
}

void Special::play_collect(void)
{
  FMOD_RESULT result;
  result = FMOD_System_PlaySound(get_system(), FMOD_CHANNEL_FREE, collect_sound,
                                 0, &collect_channel);
  ERRCHECK(result);
}

void Special::set_collect_volume(float vol)
{
  FMOD_Channel_SetVolume(collect_channel, vol);
}

void Special::set_mute(bool m)
{
  mute = m;
  FMOD_Channel_SetVolume(get_channel(), mute?0.0f:1.0f);
}

void Special::set_volume(float volume)
{
  if (!mute)
  {
    ((Character *)this)->set_volume(volume);
  }
}

void Special::go_home(Map *m)
{
  float move_x, move_y;
  
  move_x = loc[0] * TILE_WIDTH + m->get_x() - get_x();
  move_y = loc[1] * TILE_HEIGHT + m->get_y() - get_y();
  ((Drawable *)this)->move(move_x, move_y);
  number->move(move_x, move_y);
}

void Special::set_number(Drawable *num)
{
  if (num)
  {
    float x, y, nx, ny;
    
    number = num;
    get_top_left(x, y);
    number->get_top_left(nx, ny);
    number->move(-nx + x + (NUM_WIDTH / 2), -ny + y - NUM_HEIGHT);
  }
}

void Special::draw_number(void)
{
  if (number)
  {
    number->draw();
  }
}

void Special::move(float x, float y)
{
  ((Character *)this)->move(x, y);
  if (number)
  {
    number->move(x, y);
  }
}

void Special::use_ability(Map *m) {}

void Special::move_number(float x, float y){
  number->move(x, y);
}
