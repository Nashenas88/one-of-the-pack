#ifndef FLOCK__SPECIAL__H
#define FLOCK__SPECIAL__H

#include "character.h"
#include "player.h"
#include "defines.h"

enum Special_Dir_Text { PLAYER_RIGHT = 1, PLAYER_LEFT };

class Special : public Character
{
private:
  int loc[2];
  /* attribute here */
  
public:
  Special(void);
  Special(float x, float y, int num, int frames, Texture *tex,
         direc dir, bool jump, int vs, int hs, FMOD_SYSTEM *sys,
         FMOD_SOUND *so, FMOD_CHANNEL *ch);
  
  void use_ability(int dir);
  void start_following(Player *p);
  void stop_following(Player *p);
  void go_home();
  void move(int x, int y);
};

#endif // FLOCK__SPECIAL__H
