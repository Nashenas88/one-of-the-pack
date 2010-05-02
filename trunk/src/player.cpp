#include "player.h"
#include "defines.h"

Player::Player(void)
:Character(), jumping(false)
{}

Player::Player(float x, float y, int num, int frames, Texture *tex, direc dir,
               bool jump, FMOD_SYSTEM *sys, FMOD_SOUND *so, FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, 0, 0, sys, so, ch), jumping(jump),
follow(true)
{}

void Player::set_checkpoint(int x, int y)
{
  loc[0] = x;
  loc[1] = y;
}

void Player::reset(Map *m)
{
  float move_x, move_y;
  
  move_x = loc[0] * TILE_WIDTH + m->get_x() - get_x();
  move_y = loc[1] * TILE_HEIGHT + m->get_y() - get_y();
  move(move_x, move_y);
}
