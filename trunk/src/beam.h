#ifndef FLOCK__BEAM__H
#define FLOCK__BEAM__H

#include "character.h"
#include "map.h"
#include "player.h"
#include "defines.h"

typedef enum
{
  SPECIAL_RIGHT = 1,
  SPECIAL_LEFT
} Special_Dir_Text;

class Beam: public Character
{
private:
  int loc[2];
  // for ability animation
  unsigned int num_abil_frames;
  bool mute;
  FMOD_SOUND *abil_sound;
  FMOD_CHANNEL *abil_channel;
  
public:
  Beam(void);
  Beam(float x, float y, int map_x, int map_y, int num, int frames,
          int abil_frames, Texture *tex, direc dir, int hs,
          FMOD_SYSTEM *sys, FMOD_SOUND *so, FMOD_CHANNEL *ch,
          FMOD_SOUND *as, FMOD_CHANNEL *ac);
  
  unsigned int get_abil_frames(void) {return num_abil_frames;}
  
  void play_effect(void);
  bool get_mute(void) {return mute;}
  void set_mute(bool m);
  void set_volume(float volume);
  
  void move(float x);
};

#endif // FLOCK__BEAM__H
