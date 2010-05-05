#ifndef FLOCK__MOVEABLE__H
#define FLOCK__MOVEABLE__H

#include "drawable.h"
#include "defines.h"
#include "map.h"

class Kurt;

class Moveable: public Drawable
{
private:
  // gravity flag used to differentiate 
  // floating moving platforms from normal blocks
  bool gravity;
  bool rubber;
  bool resetted;
  bool freezeable;
  
  // original location
  int loc[2];
  
  // group number
  int group_num;
  
  // vertical and horizontal speed
  int v_speed, h_speed;
  
  Kurt *creator;
  vector<Moveable *> links;
  
  void setVSpeed(int vs, Map *m, vector<Moveable *> ignore);
  void setHSpeed(int hs, Map *m, vector<Moveable *> ignore);
  void setVSpeed(int vs) {v_speed = vs;}
  void setHSpeed(int hs) {h_speed = hs;}
public:
  // constructors
  Moveable(void);
  Moveable(float x, float y, int start_x, int start_y, int num, int frames,
           Texture *tex, bool g, bool r, Kurt *c);
  
  // getters and setters
  int getVSpeed(void) {return v_speed;}
  void setVSpeed(int vs, Map *m);
  int getHSpeed(void) {return h_speed;}
  void setHSpeed(int hs, Map *m);
  bool get_gravity(void) {return gravity;}
  void set_gravity(bool g) {gravity = g;}
  bool get_rubber(void) {return rubber;}
  vector<Moveable *> get_links(void) {return links;}
  void add_link(Moveable *l) {links.push_back(l);}
  int get_group_num() {return group_num;}
  void set_group_num(int gn) {group_num = gn;}
  
  bool is_freezeable(void) {return freezeable;}
  void set_freezeable(void) {freezeable = true;}
  
  void reset(Map *m);
  void move(float x, float y, Map *m);
  void move(float x, float y);
  
  // TO ONLY BE USED IN MAP.CPP
  void map_setVSpeed(int vs) {setVSpeed(vs);}
  void map_setHSpeed(int hs) {setHSpeed(hs);}
  
  bool will_collide_Dx(Drawable *o);
  bool will_collide_Dy(Drawable *o);
  bool will_collide_x(Map *m);
  bool will_collide_y(Map *m);
  bool will_collide_moveables_x(vector<Moveable *> moveables, int cut, int *collide);
  bool will_collide_moveables_y(vector<Moveable *> moveables, int cut, int *collide);
  bool will_collide_specials_x(vector<Special *> specials, int *collide);
  bool will_collide_tile(Map *m, tile_type tile, int coordinates[2]);
  bool will_collide_rubber_x(Map *m);
  bool will_collide_rubber_y(Map *m);
  
};
#endif // FLOCK__MOVEABLE__H
