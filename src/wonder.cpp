#include "wonder.h"
#include "defines.h"
#include "moveable.h"


// Wonder guy
  // -Can transform into a platform, 5 blocks wide
    // -Platform has the same properties as the punchable platforms in Level 1
    // -Main ability button toggles between platform and guy forms
            
Kurt::Kurt(void)
:Special()
{}

Kurt::Kurt(float x, float y, int map_x, int map_y, int num, int frames,
               int abil_frames, Texture *tex, direc dir, FMOD_SYSTEM *sys,
               FMOD_SOUND *music, FMOD_CHANNEL *ch, FMOD_SOUND *as,
               FMOD_CHANNEL *ac)
:Special(x, y, map_x, map_y, num, frames, abil_frames, tex, dir, 0, 0, KURT,
         sys, music, ch, as, ac), ability(false), summoned(false), move_loc(0),
num_created(0)
{
  ability = false;
}

void Kurt::use_ability(Map *m)
{
  ability = !ability;
  if (ability)
  {
    getVSpeed() == 0?setVSpeed(GRAVITY_SPEED) : setVSpeed(0);
    if (get_tex_num() != ABILITY)
    {
      set_tex_num(ABILITY);
      set_cur_frame(1);
    }
    change_direction(RIGHT);
  }
}

std::vector<Moveable*> Kurt::remove_blocks(std::vector<Moveable*> moveables)
{
  for (unsigned int i = 0; i < num_created; ++i)
  {
    delete moveables.at(move_loc);
    moveables.erase(moveables.begin() + move_loc);
  }
  
  num_created = 0;
  summoned = false;
  
  set_cur_frame(1);
  set_tex_num(SPECIAL);
  
  return moveables;
}

std::vector<Moveable*> Kurt::enable_ability(Map *m, int i, Player* p,
          std::vector<Moveable*> moveables, std::vector<Special*> specials)
{
  float old_speed = getHSpeed();
  move_loc = moveables.size();
  Moveable *temp_move;
  
  temp_move = new Moveable(get_x(), get_y(), KURT_MUSIC_NUM, 1, get_texture(), false);
  temp_move->set_cur_frame(3);
  moveables.push_back(temp_move);
  
  num_created = 1;
  
  setHSpeed(TILE_WIDTH);
  if (!will_collide_Dx(p) &&
      !will_collide_x(m) && 
      !will_collide_tile(m, PLATFORM, NULL) &&
      !will_collide_specials_x(specials, i, NULL) &&
      !will_collide_moveables_x(moveables, -1, NULL))
  {
    ++num_created;
    temp_move = new Moveable(get_x()+TILE_WIDTH, get_y(), KURT_MUSIC_NUM, 1, get_texture(), false);
    temp_move->set_cur_frame(4);
    moveables.push_back(temp_move);
    setHSpeed(2*TILE_WIDTH);
    if (!will_collide_Dx(p) &&
        !will_collide_x(m) && 
        !will_collide_tile(m, PLATFORM, NULL) &&
        !will_collide_specials_x(specials, i, NULL) &&
        !will_collide_moveables_x(moveables, -1, NULL))
    {
      ++num_created;
      temp_move = new Moveable(get_x()+TILE_WIDTH*2, get_y(), KURT_MUSIC_NUM, 1, get_texture(), false);
      temp_move->set_cur_frame(5);
      moveables.push_back(temp_move);
    }
  }
  
  setHSpeed(-TILE_WIDTH);
  if (!will_collide_Dx(p) &&
      !will_collide_x(m) && 
      !will_collide_tile(m, PLATFORM, NULL) &&
      !will_collide_specials_x(specials, i, NULL) &&
      !will_collide_moveables_x(moveables, -1, NULL))
  {
    ++num_created;
    temp_move = new Moveable(get_x()-TILE_WIDTH, get_y(), KURT_MUSIC_NUM, 1, get_texture(), false);
    temp_move->set_cur_frame(2);
    moveables.push_back(temp_move);
    setHSpeed(-TILE_WIDTH*2);
    if (!will_collide_Dx(p) &&
        !will_collide_x(m) && 
        !will_collide_tile(m, PLATFORM, NULL) &&
        !will_collide_specials_x(specials, i, NULL) &&
        !will_collide_moveables_x(moveables, -1, NULL))
    {
      ++num_created;
      moveables.push_back(new Moveable(get_x()-TILE_WIDTH*2, get_y(), KURT_MUSIC_NUM, 1, get_texture(), false));
    }
  }
  setHSpeed(old_speed);
  
  summoned = true;
  
  return moveables;
}
