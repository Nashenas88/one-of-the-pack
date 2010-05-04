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
    setVSpeed(0);
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
  Moveable *temp_move1, *temp_move2, *temp_move3, *temp_move4, *temp_move5;
  int x, y;
  m->calculate_location(this, x, y);
  
  temp_move1 = new Moveable(get_x(), get_y(), x, y, KURT_MUSIC_NUM, 1,
                           get_texture(), false, false, this);
  temp_move1->set_cur_frame(3);
  moveables.push_back(temp_move1);
  
  num_created = 1;
  
  setHSpeed(TILE_WIDTH);
  if (!will_collide_Dx(p) &&
      !will_collide_x(m) && 
      !will_collide_tile(m, PLATFORM, NULL) &&
      !will_collide_specials(specials, i) &&
      !will_collide_moveables_x(moveables, -1, NULL))
  {
    ++num_created;
    temp_move2 = new Moveable(get_x()+TILE_WIDTH, get_y(), x + 1, y,
                             KURT_MUSIC_NUM, 1, get_texture(), false, false,
                             this);
    temp_move2->set_cur_frame(4);
    moveables.push_back(temp_move2);
    
    temp_move1->add_link(temp_move2);
    temp_move2->add_link(temp_move1);
    
    setHSpeed(2*TILE_WIDTH);
    if (!will_collide_Dx(p) &&
        !will_collide_x(m) && 
        !will_collide_tile(m, PLATFORM, NULL) &&
        !will_collide_specials(specials, i) &&
        !will_collide_moveables_x(moveables, -1, NULL))
    {
      ++num_created;
      temp_move3 = new Moveable(get_x()+TILE_WIDTH*2, get_y(), x + 2, y,
                               KURT_MUSIC_NUM, 1, get_texture(), false, false,
                               this);
      temp_move3->set_cur_frame(5);
      moveables.push_back(temp_move3);
      
      temp_move2->add_link(temp_move3);
      temp_move3->add_link(temp_move2);
    }
  }
  
  setHSpeed(-TILE_WIDTH);
  if (!will_collide_Dx(p) &&
      !will_collide_x(m) && 
      !will_collide_tile(m, PLATFORM, NULL) &&
      !will_collide_specials(specials, i) &&
      !will_collide_moveables_x(moveables, -1, NULL))
  {
    ++num_created;
    temp_move4 = new Moveable(get_x()-TILE_WIDTH, get_y(), x - 1, y,
                             KURT_MUSIC_NUM, 1, get_texture(), false, false,
                             this);
    temp_move4->set_cur_frame(2);
    moveables.push_back(temp_move4);
    setHSpeed(-TILE_WIDTH*2);
    
    temp_move1->add_link(temp_move4);
    temp_move4->add_link(temp_move1);
    if (!will_collide_Dx(p) &&
        !will_collide_x(m) && 
        !will_collide_tile(m, PLATFORM, NULL) &&
        !will_collide_specials(specials, i) &&
        !will_collide_moveables_x(moveables, -1, NULL))
    {
      ++num_created;
      temp_move5 = new Moveable(get_x()-TILE_WIDTH*2, get_y(), x - 2, y,
                                KURT_MUSIC_NUM, 1, get_texture(), false, false,
                                this);
      moveables.push_back(temp_move5);
      
      temp_move4->add_link(temp_move5);
      temp_move5->add_link(temp_move4);
    }
  }
  setHSpeed((int)old_speed);
  
  summoned = true;
  
  return moveables;
}

bool Kurt::will_collide_specials(std::vector<Special*> specials, int cur)
{
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if ((int) i == cur)
    {
      continue;
    }
    if (will_collide_Dx(specials.at(i)))
    {
      return true;
    }
  }
  
  return false;
}

bool Kurt::will_collide_Dx(Drawable *d)
{
  float left_x1, left_x2;
  float right_x1, right_x2;
  float top_y1, top_y2;
  float bottom_y1, bottom_y2;
  
  get_top_left(left_x1, top_y1);
  left_x1 += getHSpeed();
  d->get_top_left(left_x2, top_y2);
  right_x1 = left_x1 + TILE_WIDTH;
  right_x2 = left_x2 + d->get_width();
  bottom_y1 = top_y1 + TILE_HEIGHT;
  bottom_y2 = top_y2 + d->get_height();
  
  if (bottom_y1 < top_y2) return false;
  if (top_y1 > bottom_y2) return false;
  
  if (right_x1 < left_x2) return false;
  if (left_x1 > right_x2) return false;
  
  return true;
}
