#include "player.h"
#include "defines.h"

Player::Player(void)
:Character(), jumping(false)
{}

Player::Player(float x, float y, int num, int frames, Texture *tex,
               direc dir, bool jump, FMOD_SYSTEM *sys,
            FMOD_SOUND *so, FMOD_CHANNEL *ch)
:Character(x, y, num, frames, tex, dir, 0, 0, sys, so, ch), jumping(jump)
{}
