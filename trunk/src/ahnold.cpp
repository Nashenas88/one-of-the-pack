#include "ahnold.h"
#include "defines.h"

Ahnold::Ahnold(void)
:Special()
{}

Ahnold::Ahnold(float x, float y, int num, int frames, int abil_frames,
               Texture *tex, direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music,
               FMOD_CHANNEL *ch)
:Special(x, y, num, frames, abil_frames, tex, dir, 0, 0, sys, music, ch)
{
  loc[0] = x;
  loc[1] = y;
  
  FMOD_RESULT result;
  
  result = FMOD_System_CreateSound(sys, RESOURCES AHNOLD_SFX, FMOD_SOFTWARE, 0, &sfx);
  ERRCHECK(result);
}

void Ahnold::use_ability(int dir, Map *m)
{
  int xd = 0;
  if (dir == RIGHT) xd = 1;
  else if (dir == LEFT) xd == -1;
  
  tile_type tile_in_dir = m->get_tile(get_x() + xd, get_y());
  if (tile_in_dir == BREAKABLE)
  {
    printf("BREAK BLOCK\n");
    m->remove(get_x() + xd, get_y());
  }
  // else if (tile_in_dir == MOVEABLE)
  // {
    // m->move_block(get_x() + xd, get_y());
  // }
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
