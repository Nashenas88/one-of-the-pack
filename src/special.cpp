
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
following(false), num_abil_frames(abil_frames), abil_sound(as), abil_channel(ac)
{
  loc[0] = x;
  loc[1] = y;
  type = t;
  set_mute(true);
}

void Special::start_following()
{
  following = true;
}

void Special::stop_following()
{
  following = false;
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

void Special::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}

void Special::use_ability(Map *m) {}