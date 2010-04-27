#ifndef FLOCK__BEAM__H
#define FLOCK__BEAM__H

#include "FMOD_includes.h"

#include "drawable.h"
#include "moveable.h"
#include "map.h"
#include "defines.h"

class Beam: public Drawable
{
private:
  // direction the character is facing
  direc direction;
  
  // used for timing animation
  int anim_delta;
  
  // vertical and horizontal speed
  int h_speed;
  
  // offsets used for collision
  float col_width, col_height;
  float col_x_offset, col_y_offset;
  
public:
  // constructors
  Beam(void);
  Beam(float x, float y, int num, int frames, Texture *tex,
            direc dir, int hs);
  
  // getters and setters
  int getDirection(void) {return direction;}
  void setDirection(direc dir) {direction = dir;}
  int getHSpeed(void) {return h_speed;}
  void setHSpeed(int hs) {h_speed = hs;}
  int get_delta(void) {return anim_delta;}
  void set_delta(int d) {anim_delta = d;}
  float get_col_x_offset(void) {return col_x_offset;}
  
  void move(float x);
  
  // check collision with other possible objects
  // (moving players are not collideables)
  bool will_collide_Dx(Drawable *o);
  bool will_collide_x(Map *m);
  bool will_collide_platform(Map *m);
  bool will_collide_tile(Map *m, tile_type tile, int coordinates[2]);
  bool will_collide_moveables_x(vector<Moveable *>specials, int cur, int *collide);
  bool will_collide_screen_x(void);
};
#endif // FLOCK__BEAM__H
