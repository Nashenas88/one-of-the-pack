#include "player.h"
#include "defines.h"

Player::Player(void)
:Character(), jumping(false)
{}

Player::Player(float x, float y, int num, int frames, Texture *tex,
               direc dir, bool jump, int vs, int hs, FMOD_SYSTEM *sys,
            FMOD_SOUND *so, FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, vs, hs, sys, so, ch), jumping(jump)
{}

// moves the player and also changes the direction
// in which he moves
void Player::move(int x, int y)
{
  ((Drawable *)this)->move(x, y);
  if (x > 0)
  {
    setDirection(RIGHT);
    set_tex_num(PLAYER_RIGHT);
  }
  else if (x < 0)
  {
    setDirection(LEFT);
    set_tex_num(PLAYER_LEFT);
  }
}
