#ifndef FLOCK__AHNOLD__H
#define FLOCK__AHNOLD__H

#include "special.h"
#include "defines.h"

class Ahnold : public Special
{
private:
  int loc[2];
  bool following;
  /* attribute here */
  
public:
  Ahnold(void);
  Ahnold(float x, float y, int num, int frames, Texture *tex,
         direc dir, bool jump, int vs, int hs, FMOD_SYSTEM *sys,
         FMOD_SOUND *so, FMOD_CHANNEL *ch);
  
  void use_ability(int dir, Map *m);
};

#endif // FLOCK__AHNOLD__H
