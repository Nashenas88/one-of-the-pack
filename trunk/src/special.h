#ifndef FLOCK__SPECIAL__H
#define FLOCK__SPECIAL__H

#include "character.h"
#include "map.h"
#include "player.h"
#include "defines.h"

typedef enum
{
  SPECIAL_RIGHT = 1,
  SPECIAL_LEFT
} Special_Dir_Text;

class Special: public Character
{
private:
  float loc[2];
  bool following;
  // for ability animation
  unsigned int num_abil_frames;
  bool mute;
  
public:
  Special(void);
  Special(float x, float y, int num, int frames, int abil_frames, Texture *tex,
         direc dir, int vs, int hs, FMOD_SYSTEM *sys, FMOD_SOUND *so,
         FMOD_CHANNEL *ch);
  
  unsigned int get_abil_frames(void) {return num_abil_frames;}
  virtual void use_ability(Map *m);
  void start_following();
  bool is_following(void) { return following; }
  void stop_following();
  bool get_mute(void) {return mute;}
  void set_mute(bool m);
  void set_volume(float volume);
  void go_home();
};

#endif // FLOCK__SPECIAL__H
