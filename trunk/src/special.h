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
  int loc[2];
  bool following;
  bool controllable;
  // for ability animation
  unsigned int num_abil_frames;
  special_type type;
  bool mute;
  FMOD_SOUND *abil_sound;
  FMOD_CHANNEL *abil_channel;
  FMOD_SOUND *collect_sound;
  FMOD_CHANNEL *collect_channel;
  Drawable *number; // number that will be displayed above special
  int jump_delta;
  
public:
  Special(void);
  Special(float x, float y, int map_x, int map_y, int num, int frames,
          int abil_frames, Texture *tex, direc dir, int vs, int hs,
          special_type t, FMOD_SYSTEM *sys, FMOD_SOUND *so, FMOD_CHANNEL *ch,
          FMOD_SOUND *as, FMOD_CHANNEL *ac, FMOD_SOUND *cs);
  ~Special(void);
  
  unsigned int get_abil_frames(void) {return num_abil_frames;}
  special_type get_type(void) {return type;}
  virtual void use_ability(Map *m);
  void start_following(void);
  bool is_following(void) { return following; }
  void stop_following(void);
  bool is_controllable(void) {return controllable;}
  
  void play_effect(void);
  void play_collect(void);
  
  void set_collect_volume(float vol);
  
  bool get_mute(void) {return mute;}
  void set_mute(bool m);
  void set_volume(float volume);
  void go_home(Map *m);
  
  int get_jump_delta(void) {return jump_delta;}
  void set_jump_delta(int d) {jump_delta = d;}
  
  Drawable *get_number(void) {return number;}
  void set_number(Drawable *num);
  
  void draw_number(void);
  void move(float x, float y);
  
  void move_number(float x, float y);
};

#endif // FLOCK__SPECIAL__H
