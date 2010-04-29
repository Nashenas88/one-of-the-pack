#ifndef FLOCK__WONDER__H
#define FLOCK__WONDER__H

#include "special.h"
#include "defines.h"
#include "player.h"

class Kurt : public Special
{  
private:
  bool ability, summoned;
  unsigned int move_loc, num_created;
  
  bool will_collide_specials(std::vector<Special*> specials, int cur);
public:
  Kurt(void);
  Kurt(float x, float y, int map_x, int map_y, int num, int frames,
         int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
         FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as, FMOD_CHANNEL *ac);
  
  bool get_ability(void) {return ability;}
  bool get_summoned(void) {return summoned;}
  std::vector<Moveable*> remove_blocks(std::vector<Moveable*> moveables);
  void use_ability(Map *m);
  std::vector<Moveable*> enable_ability(Map *m, int i, Player *p, std::vector<Moveable*> moveables, std::vector<Special*> specials);
};

#endif // FLOCK__WONDER__H
