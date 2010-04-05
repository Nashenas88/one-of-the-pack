#ifndef FLOCK__MOVEABLE__H
#define FLOCK__MOVEABLE__H

#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "drawable.h"
#include "map.h"
#include "defines.h"

class Moveable: public Drawable
{
private:
  // gravity flag used to differentiate 
  // floating moving platforms from normal blocks
  
  // vertical and horizontal speed
  int v_speed, h_speed;
  
  // offsets used for collision
  float col_width, col_height;
  float col_x_offset, col_y_offset;
    
public:
  // constructors
  Moveable(void);
  Moveable(float x, float y, int num, int frames, Texture *tex,
            direc dir, int vs, int hs);
  
  // getters and setters
  int getVSpeed(void) {return v_speed;}
  void setVSpeed(int vs) {v_speed = vs;}
  int getHSpeed(void) {return h_speed;}
  void setHSpeed(int hs) {h_speed = hs;}
  int get_animdir(void) {return animdir;}
  void set_animdir(int d) {animdir = d;}
  int get_delta(void) {return anim_delta;}
  void set_delta(int d) {anim_delta = d;}
  float get_col_y_offset(void) {return col_y_offset;}
  float get_col_x_offset(void) {return col_x_offset;}
  
  void change_direction(direc dir);
  
  void move(float x, float y);
  
  // clean all character data
  void clean(void);
};
#endif // FLOCK__MOVEABLE__H
