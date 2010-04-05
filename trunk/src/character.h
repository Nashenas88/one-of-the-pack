#ifndef FLOCK__CHARACTER__H
#define FLOCK__CHARACTER__H

#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "FMOD_includes.h"

#include "drawable.h"
#include "map.h"
#include "defines.h"

class Character: public Drawable
{
private:
  // direction the character is facing
  direc direction;
  int animdir;
  
  // used for timing animation
  int anim_delta;
  
  // vertical and horizontal speed
  int v_speed, h_speed;
  
  // offsets used for collision
  float col_width, col_height;
  float col_x_offset, col_y_offset;
  
  FMOD_SYSTEM *system;
  FMOD_SOUND *music;
  FMOD_CHANNEL *music_channel;
  
  FMOD_BOOL sound_paused;
  
public:
  // constructors
  Character(void);
  Character(float x, float y, int num, int frames, Texture *tex,
            direc dir, int vs, int hs, FMOD_SYSTEM *sys,
            FMOD_SOUND *mu, FMOD_CHANNEL *mch);
  
  // getters and setters
  int getDirection(void) {return direction;}
  void setDirection(direc dir) {direction = dir;}
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
  
  void move(float x, float y);
  
  // check collision with other possible objects
  // (moving players are not collideables)
  bool will_collide(Drawable *o);
  bool will_collide_x(Map *m);
  bool will_collide_y(Map *m);
  bool will_collide_platform(Map *m);
  bool will_collide_tile(Map *m, tile_type tile, int coordinates[2]);
  
  // sound function
  void play_sound(void);
  // you MUST call play_sound before you call pause_sound
  void pause_sound(void);
  // changes volume of music
  void set_volume(float volume);
  bool is_paused(void) {return sound_paused;}
  
  // return sound system
  FMOD_SYSTEM *get_system(void) {return system;}
  FMOD_CHANNEL *get_channel(void) {return music_channel;}
  
  // clean all character data
  void clean(void);
};
#endif // FLOCK__CHARACTER__H