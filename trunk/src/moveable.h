#ifndef FLOCK__MOVEABLE__H
#define FLOCK__MOVEABLE__H

#include "drawable.h"
#include "defines.h"
#include "map.h"

class Moveable: public Drawable
{
private:
  // gravity flag used to differentiate 
  // floating moving platforms from normal blocks
  bool gravity;
  
  // vertical and horizontal speed
  int v_speed, h_speed;
  
public:
  // constructors
  Moveable(void);
  Moveable(float x, float y, int num, int frames, Texture *tex, bool g);
  
  // getters and setters
  int getVSpeed(void) {return v_speed;}
  void setVSpeed(int vs) {v_speed = vs;}
  int getHSpeed(void) {return h_speed;}
  void setHSpeed(int hs) {h_speed = hs;}
  bool get_gravity(void) {return gravity;}
  void set_gravity(bool g) {gravity = g;}
  
  bool will_collide_Dx(Drawable *o);
  bool will_collide_Dy(Drawable *o);
  bool will_collide_x(Map *m);
  bool will_collide_y(Map *m);
  bool will_collide_moveables_x(vector<Moveable *> moveables, int cut, int *collide);
  bool will_collide_moveables_y(vector<Moveable *> moveables, int cut, int *collide);
  
};
#endif // FLOCK__MOVEABLE__H
