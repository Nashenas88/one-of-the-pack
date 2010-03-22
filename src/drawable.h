#ifndef FLOCK__DRAWABLE__H
#define FLOCK__DRAWABLE__H

#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "texture.h"

// used for determining size of drawable object
typedef enum
{
  TILE,
  TEXT,
  BACKGROUND
} D_TYPE;

class Drawable
{
private:
  float tl_x, tl_y; // (x,y) coordinates of the top left corner of object
  int width, height; // width and height of object (larger than what's viewable)
  int tex_num; // texture number on texture (sprite from top starting at 1)
               // this number is also for the y-coordinate on a non-animation
               // sprite sheet
  int num_frames; // number of frames (for animation, starts from 1)
                  // this number is also for the x-coordinate on a non-animation
                  // sprite sheet
  int cur_frame; // current frame of animation
  float corners[2][2]; // corners of sprite from texture
  Texture *texture;
  
  void reset_corners(); // re-calculates the corners when tex_num or cur_frame
                        // is changed
public:
  // constructors
  Drawable(void);
  Drawable(float x, float y, int num, int frames, D_TYPE type, Texture *tex);
  
  // draw object
  void draw(void);
  // move object by off_x in x direction and off_y in y direction
  void move(float off_x, float off_y);
  
  // getters and setters
  void get_top_left(float &x, float &y);
  void set_width(int w) {width = w;}
  int get_width(void) {return width;}
  void set_height(int h) {height = h;}
  int get_height(void) {return height;}
  void set_tex_num(int tn);
  int get_tex_num(void) {return tex_num;}
  void set_num_frames(int nf) {num_frames = nf;}
  int get_num_frames(void) {return num_frames;}
  void set_cur_frame(int cf);
  int get_cur_frame(void) {return cur_frame;}
};

#endif // FLOCK__DRAWABLE__H