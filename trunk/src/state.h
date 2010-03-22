#ifndef FLOCK__STATE__H
#define FLOCK__STATE__H

#include "FMOD_includes.h"

class State
{
private:
  FMOD_SYSTEM *system;
public:
  State(void);
  State(FMOD_SYSTEM *sys);
  
  virtual void draw(void);
  
  virtual void update(int &delta);
  void state_update(void);
  
  virtual void key_pressed(unsigned char key, int x, int y);
  virtual void key_released(unsigned char key, int x, int y);
  
  FMOD_SYSTEM *get_system(void) {return system;}
  
  virtual void clean(void);
  void state_clean(void);
};
#endif // FLOCK__STATE__H
