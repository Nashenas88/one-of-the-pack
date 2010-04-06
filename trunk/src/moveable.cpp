#include "moveable.h"

Moveable::Moveable(void)
:Drawable(),gravity(false),v_speed(0),h_speed(0) {}

Moveable::Moveable(float x, float y, int num, int frames, Texture *tex,
                   bool g)
:Drawable(x, y, num, frames, TILE, tex), gravity(g), v_speed(0), h_speed(0) {}