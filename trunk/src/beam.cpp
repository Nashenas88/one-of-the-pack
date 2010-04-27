
#include "beam.h"
#include "defines.h"

Beam::Beam(void)
:Character()
{}

Beam::Beam(float x, float y, int map_x, int map_y, int num, int frames,
                 int abil_frames, Texture *tex, direc dir, int hs,
                 FMOD_SYSTEM *sys, FMOD_SOUND *so,
                 FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Character(x, y, num, frames, tex, dir, 0, hs, sys, so, ch),
num_abil_frames(abil_frames), abil_sound(as), abil_channel(ac)
{
  loc[0] = map_x;
  loc[1] = map_y;
  set_mute(true);
}

void Beam::play_effect(void)
{
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(get_system(), FMOD_CHANNEL_FREE, abil_sound,
                                 0, &abil_channel);
  ERRCHECK(result);
}

void Beam::set_mute(bool m)
{
  mute = m;
  FMOD_Channel_SetVolume(get_channel(), mute?0.0f:1.0f);
}

void Beam::set_volume(float volume)
{
  if (!mute)
  {
    ((Character *)this)->set_volume(volume);
  }
}

void Beam::move(float x)
{
  ((Character *)this)->move(x, 0);
}
