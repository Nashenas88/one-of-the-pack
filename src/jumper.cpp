#include "jumper.h"
#include "defines.h"

Jumper::Jumper(void)
:Special()
{}

Jumper::Jumper(float x, float y, int num, int frames, int abil_frames,
               Texture *tex, direc dir, FMOD_SYSTEM *sys, FMOD_SOUND *music,
               FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac)
:Special(x, y, num, frames, abil_frames, tex, dir, 0, 0, JUMPER, sys, music,
         ch, as, ac), link(NULL)
{
  loc[0] = x;
  loc[1] = y;
}

void Jumper::use_ability(Map *m)
{
  if (get_tex_num() != ABILITY)
  {
    int old_speed;
    
    old_speed = getVSpeed();
    setVSpeed(GRAVITY_SPEED);
    
    if (will_collide_y(m) ||
        will_collide_platform(m) ||
        will_collide_tile(m, LADDER, NULL) ||
        will_collide_moveables_y(m->get_moveables(), -1, NULL))
    {
      play_effect();
      setVSpeed(-JUMP_HEIGHT);
      set_tex_num(ABILITY);
      set_cur_frame(1);
      return;
    }
    setVSpeed(old_speed);
  }
}

void Jumper::start_following(Player *p)
{
  following = true;
}

void Jumper::stop_following(Player *p)
{
  following = false;
}

void Jumper::go_home(void)
{
  ((Drawable *)this)->move(loc[0]-get_x(), loc[1]-get_y());
}
