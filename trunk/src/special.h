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
  
public:
  Special(void);
  Special(float x, float y, int num, int frames, Texture *tex,
         direc dir, int vs, int hs, FMOD_SYSTEM *sys, FMOD_SOUND *so,
         FMOD_CHANNEL *ch);
  
  virtual void use_ability(int dir, Map *m){}
  void start_following(Player *p);
  bool is_following(void) { return following; }
  void stop_following(Player *p);
  void go_home();
  void move(float x, float y);
};

#endif // FLOCK__SPECIAL__H
