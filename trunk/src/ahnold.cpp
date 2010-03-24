#include "ahnold.h"
#include "defines.h"

Ahnold::Ahnold(void)
:Special()
{}

Ahnold::Ahnold(float x, float y, int num, int frames, Texture *tex, direc dir, 
               FMOD_SYSTEM *sys, FMOD_SOUND *music, FMOD_CHANNEL *ch)
:Special(x, y, num, frames, tex, dir, 0, 0, sys, music, ch)
{
  loc[0] = x;
  loc[1] = y;
  
  FMOD_RESULT result;
  
  result = FMOD_System_CreateSound(sys, RESOURCES AHNOLD_SFX, FMOD_SOFTWARE, 0, &sfx);
  ERRCHECK(result);
}

void Ahnold::use_ability(int dir, Map *m)
{
}

void Ahnold::start_following(Player *p)
{
  following = true;
}

void Ahnold::stop_following(Player *p)
{
  following = false;
}

void Ahnold::go_home()
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}

// moves the player and also changes the direction
// in which he moves
void Ahnold::move(int x, int y)
{
  ((Drawable *)this)->move(x, y);
  if (x > 0)
  {
    setDirection(RIGHT);
    set_tex_num(SPECIAL_RIGHT);
  }
  else if (x < 0)
  {
    setDirection(LEFT);
    set_tex_num(SPECIAL_RIGHT);
  }
}
