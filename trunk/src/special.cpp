
#include "special.h"
#include "defines.h"

Special::Special(void)
:Character()
{}

Special::Special(float x, float y, int num, int frames, int abil_frames,
                 Texture *tex, direc dir, int vs, int hs, special_type t, 
                 FMOD_SYSTEM *sys, FMOD_SOUND *so, FMOD_CHANNEL *ch, 
                 FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch),
following(true), controllable(false), num_abil_frames(abil_frames),
abil_sound(as), abil_channel(ac), number(NULL)
{
  loc[0] = x;
  loc[1] = y;
  type = t;
  set_mute(true);
}

void Special::start_following(void)
{
  following = true;
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

void Special::go_home(void)
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
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

/*
void Special::draw(void)
{
  ((Character *)this)->draw();
  if (number)
  {
    float x, y;
    number->get_top_left(x, y);
    number->draw();
  }
}*/

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
