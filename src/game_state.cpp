#include <algorithm>

#include "math.h"
#include "game_state.h"
#include "jumper.h"
#include "ahnold.h"
#include "engineer.h"
#include "wonder.h"

bool sort_by_height (Special *i, Special *j);

Game_State::Game_State(void)
:p(NULL), c(NULL), map(NULL), gravity(true), collision(true) {}

Game_State::Game_State(Player *pl, Map *m, vector<Moveable *> mvs,
                       vector<Special *> sps, vector<Drawable *> nums,
                       FMOD_SYSTEM *system)
:State(system), p(pl), c(pl), map(m), moveables(mvs), specials(sps),
numbers(nums), next_special(0), gravity(true), collision(true), w(0), a(0),
s(0), d(0), last_x(0), last_y(0), map_slide_effect(SLIDE_COUNTER), last_key(0),
key_held(0), jump_delta(-1), debug(false) {}

// this draws everything to the screen
void Game_State::draw(void)
{
  vector<Special *> sorted_draw;
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    sorted_draw.push_back(specials.at(i));
  }
  
  for (unsigned int i = 0; i < sorted_draw.size(); ++i)
  {
    if (!sorted_draw.at(i)->is_following())
    {
      sorted_draw.push_back(sorted_draw.at(i));
      sorted_draw.erase(sorted_draw.begin() + i);
    }
  }
  
  // first the map
  map->draw();
  // then the moveables
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->draw();
  }
  
  for (unsigned int i = 0; i < beams.size(); ++i)
  {
    beams.at(i)->draw();
  }
  
  // then the specials, except for the one you are controlling
  // the closer to the player, the higher up they should be drawn
  // (1st special should be drawn over 2nd special, etc.)
  for (unsigned int i = sorted_draw.size() - 1; i < sorted_draw.size() ; --i)
  {
    sorted_draw.at(i)->draw();
  }
  
  for (unsigned int i = 0; i < specials.size() ; ++i)
  {
    // don't draw number of special you're controlling
    if (c == specials.at(i))
    {
      continue;
    }
    specials.at(i)->draw_number();
  }
  // then the player
  p->draw();
}

// this is called every time the scene
// needs to be updated
void Game_State::update(int &delta)
{
  // are we hitting the goal?
  // no need to do anything else if we are
  if (collision && p->will_collide_tile(map, GOAL, NULL))
  {
    delta = -1;
    return;
  }
  
  vector<int> to_delete;
  int coords[2];
  int which;
  int j;
  
  for (int i = 0; i < (int) beams.size(); ++i)
  {
    for (j = BLOCK1; j < LADDER; ++j)
    {
      if(beams.at(i)->will_collide_tile(map, (tile_type) j, coords))
      {
        beams.at(i)->play_effect();
        delete beams.at(i);
        to_delete.push_back(i);
        
        if (map->get_tile(coords[0], coords[1]) == CIRCUIT)
        {
          map->convert_circuit(coords[0], coords[1]);
          break;
        }
        else if (map->get_coll(coords[0], coords[1]) == CIRCUIT)
        {
          map->unconvert_circuit(coords[0], coords[1]);
          break;
        }
        
        // make sure the blocks we'll be checking are in the map
        if (coords[0] - 1 < 0)
        {
          coords[0] = 0;
        }
        if (coords[0] + 1 >= map->get_width())
        {
          coords[0] = map->get_width() - 1;
        }
        if (coords[1] - 1 < 0)
        {
          coords[1] = 0;
        }
        if (coords[1] + 1 >= map->get_height())
        {
          coords[1] = map->get_height() - 1;
        }
        
        // try to convert the blocks
        for (int k = coords[0] - 1; k < coords[0] + 2; ++k)
        {
          for (int l = coords[1] - 1; l < coords[1] + 2; ++l)
          {
            if (!map->make_rubber(k, l))
            {
              map->return_from_rubber(k, l);
            }
          }
        }
        break;
      }
    }
    if (j == LADDER)
    {
      if (beams.at(i)->will_collide_moveables_x(moveables, -1, &which))
      {
        moveables.at(which)->set_gravity(!moveables.at(which)->get_gravity());
        beams.at(i)->play_effect();
        delete beams.at(i);
        to_delete.push_back(i);
      }
      else if (beams.at(i)->will_collide_x(map))
      {
        beams.at(i)->play_effect();
        delete beams.at(i);
        to_delete.push_back(i);
      }
    }
  }
  for(unsigned int i = 0; i < to_delete.size(); ++i)
  {
    beams.erase(beams.begin() + to_delete.at(i));
    for (unsigned int j = i; j < to_delete.size(); ++j)
    {
      to_delete.at(j) = to_delete.at(j) - 1;
    }
  }
  
  for (unsigned int i = 0; i < beams.size(); ++i)
  {
    beams.at(i)->move(beams.at(i)->getHSpeed(), 0);
  }
  
  // if the specials are hitting any black holes, reset their positions
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if(specials.at(i)->will_collide_tile(map, BLACK_HOLE, NULL) &&
       !specials.at(i)->will_collide_y(map))
    {
      specials.at(i)->go_home(map);
    }
  }
  
  float x, y, map_x, map_y;
  int num_following = 0;
  bool player_movey;
  
  c->get_top_left(x, y);
  map->get_top_left(map_x, map_y);
  
  // reset everything's horizontal speeds
  p->setHSpeed(0);
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->setHSpeed(0);
  }
  
  // change speed of controlled player
  // if keys are being pressed
  if (last_x && a && !(c != p && ((Special*)c)->get_type() == KURT &&
                       ((Kurt*)c)->get_ability()))
  {
    c->setHSpeed(-PLAYER_SPEED);
  }
  else if (d && !(c != p && ((Special*)c)->get_type() == KURT &&
                  ((Kurt*)c)->get_ability()))
  {
    c->setHSpeed(PLAYER_SPEED);
  }
  
  // if there is no collision, allow the
  // controlled character to fly around
  if (!collision)
  {
    if (last_y && w)
    {
      c->setVSpeed(-PLAYER_SPEED);
    }
    else if (s)
    {
      c->setVSpeed(PLAYER_SPEED);
    }
  }
  
  // reset gravity for everything
  if (gravity)
  {
    int x, y;
    map->calculate_location(p, x, y);
    
    if (p->get_bounce() && p->get_jump() && y >= p->get_jump())
    {
      p->setVSpeed(-GRAVITY_SPEED);
    }
    else
    {
      if (!p->will_collide_y(map) &&
          !p->will_collide_tile(map, LADDER, NULL) &&
          !p->will_collide_moveables_y(moveables, -1, NULL) &&
          !p->will_collide_platform(map))
      {
        if (!p->get_jump())
        {
          p->set_jump(y);
        }
        else if (y <= p->get_jump())
        {
          p->set_jump(0);
          p->set_bounce(false);
        }
      }
      p->setVSpeed(GRAVITY_SPEED);
    }
    
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (specials.at(i)->get_type() == KURT &&
          ((Kurt*)specials.at(i))->get_ability())
      {
        continue;
      }
      
      map->calculate_location(specials.at(i), x, y);
      if (specials.at(i)->get_bounce() && specials.at(i)->get_jump() &&
          y >= specials.at(i)->get_jump())
      {
        specials.at(i)->setVSpeed(-GRAVITY_SPEED);
      }
      else
      {
        if (!specials.at(i)->get_jump())
        {
          specials.at(i)->set_jump(y);
        }
        else if (y <= specials.at(i)->get_jump())
        {
          specials.at(i)->set_jump(0);
          specials.at(i)->set_bounce(false);
        }
        
        // if we are not a jumper reset gravity
        if (specials.at(i)->get_type() != JUMPER)
        {
          specials.at(i)->setVSpeed(GRAVITY_SPEED);
        }
        // if we are a jumper and we are not jumping, reset gravity
        else if (specials.at(i)->get_tex_num() == SPECIAL)
        {
          specials.at(i)->setVSpeed(GRAVITY_SPEED);
        }
      }
    }
    if (!(c != p && ((Special*)c)->get_type() == KURT &&
        ((Kurt*)c)->get_ability()))
    {
      if (jump_delta != -1 && !(c->get_bounce() && c->getVSpeed() < 0))
      {
        c->setVSpeed(-JUMP_HEIGHT);
        ++jump_delta;
      }
      if (jump_delta > JUMP_DELAY && !(c->get_bounce() && c->getVSpeed() < 0))
      {
        jump_delta = -1;
      }
    }
  }
  
  bool follow_spec = false, follow_move = false;;
  int follow = -1;
  int mov_follow = -1;
  int temp_speed;
  vector<Moveable *> links, ignore;
  
  // check for collision on everyone
  // collision is always on for moveables
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    if (moveables.at(i)->get_gravity())
    {
      moveables.at(i)->setVSpeed(GRAVITY_SPEED, map);
    }
    if(moveables.at(i)->will_collide_y(map))
    {
      moveables.at(i)->setVSpeed(0, map);
    }
    moveables.at(i)->move(0, moveables.at(i)->getVSpeed());
    
    if(moveables.at(i)->will_collide_x(map) ||
       moveables.at(i)->will_collide_specials_x(specials, NULL))
    {
      moveables.at(i)->setHSpeed(0, map);
    }
    if(moveables.at(i)->will_collide_rubber_x(map))
    {
      moveables.at(i)->setHSpeed(-moveables.at(i)->getHSpeed(), map);
    }
    if(moveables.at(i)->will_collide_rubber_y(map))
    {
      moveables.at(i)->setVSpeed(-moveables.at(i)->getVSpeed(), map);
    }
  }
  if (collision)
  {
    // check vertical collision for specials
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (specials.at(i)->get_type() == KURT &&
          ((Kurt*)specials.at(i))->get_ability())
      {
        continue;
      }
      
      // if we are controlling this special, then just change the speed
      // the movement is done when we check for map collision
      if (c == specials.at(i))
      {
        if (specials.at(i)->will_collide_y(map) ||
            specials.at(i)->will_collide_tile(map, LADDER, NULL) ||
            specials.at(i)->will_collide_platform(map) ||
            specials.at(i)->will_collide_specials_y(specials, i, NULL) ||
            specials.at(i)->will_collide_moveables_y(moveables, -1, NULL))
        {
          c->setVSpeed(0);
          c->set_jump(0);
          c->set_bounce(false);
        }
        else if (c->will_collide_rubber_y(map))
        {
          c->setVSpeed(-GRAVITY_SPEED);
          c->set_bounce(true);
        }
        if (specials.at(i)->will_collide_x(map) ||
            specials.at(i)->will_collide_moveables_x(moveables, -1, NULL))
        {
          c->setHSpeed(0);
        }
        continue;
      }
      
      // move specials in the y if they can
      if (specials.at(i)->will_collide_y(map) ||
          specials.at(i)->will_collide_tile(map, LADDER, NULL) ||
          specials.at(i)->will_collide_platform(map))
      {
        specials.at(i)->setVSpeed(0);
        specials.at(i)->set_jump(0);
        specials.at(i)->set_bounce(false);
      }
      else if (specials.at(i)->will_collide_rubber_y(map))
      {
        specials.at(i)->setVSpeed(-GRAVITY_SPEED);
        specials.at(i)->set_bounce(true);
      }
    }
    
    // we need to check for following
    vector<Special *>sorted_specials;
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      sorted_specials.push_back(specials.at(i));
    }
    sort(sorted_specials.begin(), sorted_specials.end(), sort_by_height);
    
    for (unsigned int i = 0; i < sorted_specials.size();  ++i)
    {
      if (sorted_specials.at(i)->get_type() == KURT &&
          ((Kurt*)sorted_specials.at(i))->get_ability())
      {
        continue;
      }
      
      temp_speed = sorted_specials.at(i)->getVSpeed();
      sorted_specials.at(i)->setVSpeed(GRAVITY_SPEED);
      follow_move = sorted_specials.at(i)->will_collide_moveables_y(moveables, -1, &mov_follow);
      follow_spec = sorted_specials.at(i)->will_collide_specials_y(sorted_specials, i, &follow);
      sorted_specials.at(i)->setVSpeed(temp_speed);
      
      // if on special or moveable, move with them
      if (follow_spec && follow >= 0 && !p->will_collide_Dx(sorted_specials.at(follow)))
      {
        if (c != sorted_specials.at(i))
        {
          sorted_specials.at(i)->setHSpeed(sorted_specials.at(follow)->getHSpeed());
          sorted_specials.at(i)->setVSpeed(sorted_specials.at(follow)->getVSpeed());
        }
        else
        {
          c->setHSpeed(c->getHSpeed() + sorted_specials.at(follow)->getHSpeed());
          c->setVSpeed(c->getVSpeed() + sorted_specials.at(follow)->getVSpeed());
        }
      }
      else if (follow_move && mov_follow >= 0)
      {
        sorted_specials.at(i)->setHSpeed(sorted_specials.at(i)->getHSpeed() +
                                  moveables.at(mov_follow)->getHSpeed());
        sorted_specials.at(i)->setVSpeed(moveables.at(mov_follow)->getVSpeed());
      }
      if ((sorted_specials.at(i)->get_type() == JUMPER &&
            sorted_specials.at(i)->get_tex_num() == ABILITY &&
            !(sorted_specials.at(i)->will_collide_y(map) ||
              sorted_specials.at(i)->will_collide_tile(map, LADDER, NULL))) ||
           (c == sorted_specials.at(i) && jump_delta != -1 &&
            !(c->will_collide_y(map) ||
              c->will_collide_tile(map, LADDER, NULL))))
      {
        sorted_specials.at(i)->setVSpeed(-JUMP_HEIGHT);
      }
      if (sorted_specials.at(i)->will_collide_moveables_y(moveables, -1, NULL))
      {
        sorted_specials.at(i)->setVSpeed(0);
      }
    }
    // move specials in the y
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (c == specials.at(i))
      {
        continue;
      }
      specials.at(i)->move(0, specials.at(i)->getVSpeed());
    }
    
    gravity = !p->will_collide_tile(map, LADDER, NULL);
    if (!gravity)
    {
      if (c == p && last_y && w)
      {
        p->setVSpeed(-PLAYER_SPEED);
      }
      else if (c == p && s)
      {
        p->setVSpeed(PLAYER_SPEED);
      }
      else
      {
        p->setVSpeed(0);
      }
      
      int temp;
      temp = p->getVSpeed();
      p->move(0, temp);
      p->setVSpeed(PLAYER_SPEED);
      if (!p->will_collide_tile(map, LADDER, NULL))
      {
        p->setVSpeed(0);
        p->set_jump(0);
        p->set_bounce(false);
      }
      else
      {
        p->setVSpeed(temp);
      }
      p->move(0, -temp);
    }
    else if (p->will_collide_rubber_y(map))
    {
      p->setVSpeed(-GRAVITY_SPEED);
      p->set_bounce(true);
    }
    
    // change falling speed to PLAYER_SPEED when too close to floor
    // this is to deal with case when you are 10 + 20 * i pixels from
    // the ground and you leave the ladder
    // alse check to see if we should follow any specials or moveables
    // because we are standing on them
    
    temp_speed = p->getVSpeed();
    p->setVSpeed(GRAVITY_SPEED);
    follow_move = p->will_collide_moveables_y(moveables, -1, &mov_follow);
    follow_spec = p->will_collide_specials_y(specials, -1, &follow);
    if (c == p)
    {
      follow_spec = follow_spec && !p->will_collide_tile(map, LADDER, NULL);
    }
    p->setVSpeed(temp_speed);
    
    // if on special or moveable, move with them
    if (follow_spec && follow >= 0 && !p->will_collide_Dx(specials.at(follow)))
    {
      p->setHSpeed(p->getHSpeed() + specials.at(follow)->getHSpeed());
      if (p->getVSpeed() >= 0)
      {
        p->setVSpeed(specials.at(follow)->getVSpeed());
      }
    }
    else if (follow_move && mov_follow >= 0)
    {
      p->setHSpeed(p->getHSpeed() + moveables.at(mov_follow)->getHSpeed());
      if (p->getVSpeed() >= 0)
      {
        p->setVSpeed(moveables.at(mov_follow)->getVSpeed());
      }
    }
    
    player_movey = !p->will_collide_y(map) && !p->will_collide_platform(map);
    
    if (!player_movey && !follow_spec && !follow_move &&
        p->getVSpeed() == GRAVITY_SPEED)
    {
      p->setVSpeed(PLAYER_SPEED);
      // check here to see if we'll follow a special
      follow_spec = p->will_collide_specials_y(specials, -1, &follow);
      if (c == p)
      {
        follow_spec = follow_spec && !p->will_collide_tile(map, LADDER, NULL);
      }
      follow_move = p->will_collide_moveables_y(moveables, -1, &mov_follow);
      
      // if on special or moveable, move with them
      if (follow_spec && follow >= 0 && !p->will_collide_Dx(specials.at(follow)))
      {
        p->setHSpeed(p->getHSpeed() + specials.at(follow)->getHSpeed());
        if (p->getVSpeed() >= 0)
        {
          p->setVSpeed(specials.at(follow)->getVSpeed());
        }
      }
      else if (follow_move && mov_follow >= 0)
      {
        p->setHSpeed(p->getHSpeed() + moveables.at(mov_follow)->getHSpeed());
        if (p->getVSpeed() >= 0)
        {
          p->setVSpeed(moveables.at(mov_follow)->getVSpeed());
        }
      }
      
      player_movey = !p->will_collide_y(map) && !p->will_collide_platform(map) &&
                     !p->will_collide_tile(map, LADDER, NULL);
    }
    if (!player_movey)
    {
      p->setVSpeed(0);
      p->set_jump(0);
      p->set_bounce(false);
    }
    
    if (c != p)
    {
      p->setHSpeed(p->will_collide_x(map) ||
                   p->will_collide_moveables_x(moveables, -1, NULL)?
                   0 : p->getHSpeed());
    }
    
    // check for horizontal collision with human controlled character
    c->setHSpeed(c->will_collide_x(map) ||
                 c->will_collide_moveables_x(moveables, -1, NULL)?
                 0 : c->getHSpeed());
  }
  
  // check for following in specials
  float dx, dy, dist;
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    // don't try and follow if human is controlling
    // this special
    if (c == specials.at(i))
    {
      continue;
    }
    // calculating distance from player
    dx = (specials.at(i)->get_x() - p->get_x());
    dy = (specials.at(i)->get_y() - p->get_y());
    dist = sqrt((dx*dx)+(dy*dy));
    if (specials.at(i)->is_following())
    {
      if (specials.at(i)->is_controllable())
      {
        ++num_following;
        
        // if the special is not too close and it's not colliding with anything
        // then change its speed
        if (dist > TOO_CLOSE + (num_following-1)*TILE_WIDTH && follow != (int)i)
        {
          specials.at(i)->setHSpeed(!dx?specials.at(i)->getHSpeed():(dx<0?PLAYER_SPEED:-PLAYER_SPEED));
          if (specials.at(i)->will_collide_x(map) ||
              specials.at(i)->will_collide_moveables_x(moveables, -1, NULL))
          {
            specials.at(i)->setHSpeed(0);
          }
        }
      }
      // if it's controllable and it's close enough, then make it start
      // following the player
      else if (dist < FOLLOW_DIST)
      {
        specials.at(i)->start_following();
        // turn on the music
        if (specials.at(i)->get_mute())
        {
          specials.at(i)->set_mute(false);
        }
        // this is so that the specials follow you in the order that
        // you collect them
        if (i > (unsigned int) next_special)
        {
          Special *temp_special;
          temp_special = specials.at(i);
          specials.at(i) = specials.at(next_special);
          specials.at(next_special) = temp_special;
          ++next_special;
        }
        else if (i == (unsigned int) next_special)
        {
          ++next_special;
        }
        specials.at(next_special - 1)->set_number(numbers.at(next_special - 1));
      }
    }
  }
  
  // move specials in the x
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    // if this special is being controlled, then don't move it. this will
    // be done when we check for collision with the map
    if (c == specials.at(i))
    {
      continue;
    }
    //if (!specials.at(i)->will_collide_x(map))
    //{
      specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
    //}
  }
  
  // move moveables
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    ((Drawable *)moveables.at(i))->move(moveables.at(i)->getHSpeed(), 0);
  }
  
  c->move(c->getHSpeed(), c->getVSpeed());
  if (c != p)
  {
    ((Special *)c)->get_number()->move(c->getHSpeed(), c->getVSpeed());
    p->move(p->getHSpeed(), p->getVSpeed());
  }
  
  if(map_slide_effect)
  {
    float center_x, center_y;
    float control_x, control_y;
    float move_x, move_y;
    float offset_x, offset_y;
    int mx, my, i = -1;
    float speed;
    bool will_collide;
    if (c != p)
    {
      for (unsigned int j = 0; j < specials.size(); ++j)
      {
        if ((Special*)c == specials.at(j))
        {
          i = j;
          break;
        }
      }
    }
    
    speed = c->getVSpeed();
    c->get_top_left(control_x, control_y);
    center_x = SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f;
    if (c != p && ((Special*)c)->get_type() == JUMPER && c->getVSpeed() < 0)
    {
      center_y = 3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT;
    }
    else if (speed > 0 && !c->will_collide_tile(map, LADDER, NULL) &&
        (c->setVSpeed((int)(TILE_HEIGHT * 1.3f)),
         will_collide = !c->will_collide_y(map) &&
         !c->will_collide_tile(map, PLATFORM, NULL) &&
         !c->will_collide_tile(map, LADDER, NULL) &&
         !c->will_collide_moveables_y(moveables, -1, NULL) &&
         !c->will_collide_specials_y(specials, i, NULL) &&
         !c->will_collide_rubber_y(map),
         c->setVSpeed((int) speed),will_collide))
    {
      center_y = SCREEN_HEIGHT / 3.0f - TILE_HEIGHT;
    }
    else
    {
    center_y = 3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT;
    }
    offset_x = center_x - control_x;
    offset_y = center_y - control_y;
    
    if (map_slide_effect > 3 * SLIDE_COUNTER / 4)
    {
      move_x = offset_x / (2 * map_slide_effect);
      move_y = offset_y / (2 * map_slide_effect);
      --map_slide_effect;
    }
    else
    {
      move_x = offset_x / map_slide_effect;
      move_y = offset_y / map_slide_effect;
    }
    
    mx = (int)move_x;
    my = (int)move_y;
    
    if (mx < SMALLEST_MOVEMENT && mx > -SMALLEST_MOVEMENT)
    {
      mx = 0;
    }
    if (my < SMALLEST_MOVEMENT && my > -SMALLEST_MOVEMENT)
    {
      my = 0;
    }
    
    if (mx || my)
    {
      map->move(mx, my);
      ((Drawable *)p)->move(mx, my);
      for (unsigned int i = 0; i < specials.size(); ++i)
      {
        ((Drawable *)specials.at(i))->move(mx, my);
        if (specials.at(i)->get_number())
        {
          specials.at(i)->get_number()->move(mx, my);
        }
      }
      for (unsigned int i = 0; i < moveables.size(); ++i)
      {
        ((Drawable *)moveables.at(i))->move(mx, my);
      }
      for (unsigned int i = 0; i < beams.size(); ++i)
      {
        ((Drawable *)beams.at(i))->move(mx, my);
      }
    }
  }
  
  // animation for plyer
  if (c == p && p->get_num_frames() > 1 && p->getHSpeed() != 0 &&
      (a || d))
  {
    if (delta > DELTA_DELAY)
    {
      int current_frame;
      
      delta = 0;
      current_frame = p->get_cur_frame();
      ++current_frame;
      current_frame %= p->get_num_frames() + 1;
      if(!current_frame)
      {
        ++current_frame;
      }
      p->set_cur_frame(current_frame);
    }
    else
    {
      ++delta;
    }
  }
  else
  {
    p->set_cur_frame(1);
  }
  
  // animation for specials
  // for the jumper, when jumping and at the last animation
  // turn gravity back on
  int a_delta;
  unsigned int cur_frame;
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    a_delta = specials.at(i)->get_delta();
    if (specials.at(i)->get_tex_num() >= ABILITY)
    {
      if (specials.at(i)->get_type() != ENGINEER &&
          specials.at(i)->get_type() != AHNOLD &&
          specials.at(i)->get_type() != PARIS &&
          a_delta > SPECIAL_DELTA_DELAY)
      {
        specials.at(i)->set_delta(0);
        cur_frame = specials.at(i)->get_cur_frame();
        if (cur_frame < specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(++cur_frame);
        }
        else if (cur_frame == specials.at(i)->get_abil_frames())
        {
          if (specials.at(i)->get_type() == KURT &&
                   !((Kurt *)specials.at(i))->get_summoned())
          {
            moveables = ((Kurt *)specials.at(i))->enable_ability(map, i, p, moveables, specials);
            map->set_moveables(moveables);
          }
          if (specials.at(i)->get_type() == KURT &&
              ((Kurt*)specials.at(i))->get_ability())
          {
            if (cur_frame < specials.at(i)->get_num_frames())
            {
              specials.at(i)->set_cur_frame(cur_frame + 1);
            }
            else
            {
              specials.at(i)->set_cur_frame(1);
              specials.at(i)->set_tex_num((specials.at(i)->get_tex_num() - 1) %
                                          KURT_ABILITY_FRAMES + 2);
            }
          }
          else
          {
            specials.at(i)->set_cur_frame(1);
            specials.at(i)->set_tex_num(SPECIAL);
          }
        }
      }
      else if ((specials.at(i)->get_type() == ENGINEER &&
                a_delta > ENGINEER_DELTA_DELAY) ||
               (specials.at(i)->get_type() == AHNOLD &&
                a_delta > AHNOLD_DELTA_DELAY) ||
               (specials.at(i)->get_type() == PARIS &&
                a_delta > PARIS_DELTA_DELAY))
      {
        specials.at(i)->set_delta(0);
        cur_frame = specials.at(i)->get_cur_frame();
        if (cur_frame < specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(++cur_frame);
        }
        else
        {
          if(specials.at(i)->get_type() == AHNOLD)
          {
            if(cur_frame == specials.at(i)->get_num_frames())
            {
              ((Ahnold *)specials.at(i))->enable_ability(map);
            }
          }
          else if (specials.at(i)->get_type() == ENGINEER &&
                  cur_frame == specials.at(i)->get_num_frames())
          {
            beams.push_back(((Engineer *) specials.at(i))->enable_ability(map));
          }
          else if (specials.at(i)->get_type() == PARIS)
          {
            int coords[2];
            float temp_speed;
            temp_speed = specials.at(i)->getHSpeed();
            if (specials.at(i)->getDirection() == RIGHT)
            {
              specials.at(i)->setHSpeed(PARIS_KISS_RANGE);
            }
            else
            {
              specials.at(i)->setHSpeed(-PARIS_KISS_RANGE);
            }
            if (specials.at(i)->will_collide_tile(map, BOUNCER_CLOSED_R, coords) ||
                specials.at(i)->will_collide_tile(map, BOUNCER_CLOSED_L, coords))
            {
              map->open_bouncer(coords[0], coords[1]);
            }
            specials.at(i)->setHSpeed((int)temp_speed);
          }
          specials.at(i)->set_delta(-1);
        }
      }
      else if ((specials.at(i)->get_type() == ENGINEER &&
                a_delta < 0 && a_delta < -ENGINEER_DELTA_DELAY) ||
               (specials.at(i)->get_type() == AHNOLD &&
                a_delta < 0 && a_delta < -ENGINEER_DELTA_DELAY) ||
               (specials.at(i)->get_type() == PARIS &&
                a_delta < 0 && a_delta < PARIS_DELTA_DELAY))
      {
        specials.at(i)->set_delta(-1);
        cur_frame = specials.at(i)->get_cur_frame();
        if (cur_frame == 1)
        {
          specials.at(i)->set_delta(0);
          specials.at(i)->set_tex_num(SPECIAL);
        }
        else
        {
          specials.at(i)->set_cur_frame(--cur_frame);
        }
      }
      else
      {
        specials.at(i)->set_delta(++a_delta);
      }
    }
    else if (specials.at(i)->get_num_frames() > 1 &&
             !(specials.at(i)->getHSpeed() < PLAYER_SPEED &&
               specials.at(i)->getHSpeed() > -PLAYER_SPEED))
    {
      if (a_delta > DELTA_DELAY)
      {
        int current_frame;
        
        specials.at(i)->set_delta(0);
        current_frame = specials.at(i)->get_cur_frame();
        ++current_frame;
        current_frame %= specials.at(i)->get_num_frames() + 1;
        if(!current_frame)
        {
          ++current_frame;
        }
        specials.at(i)->set_cur_frame(current_frame);
      }
      else
      {
        specials.at(i)->set_delta(++a_delta);
      }
    }
    else
    {
      specials.at(i)->set_cur_frame(1);
    }
  }
  
  // animation for beams
  for (unsigned int i = 0; i < beams.size(); ++i)
  {
    a_delta = beams.at(i)->get_delta();
    if (a_delta > BEAM_DELTA_DELAY)
    {
      cur_frame = beams.at(i)->get_cur_frame();
      if (cur_frame == beams.at(i)->get_num_frames())
      {
        beams.at(i)->set_cur_frame(1);
      }
      else
      {
        beams.at(i)->set_cur_frame(++cur_frame);
      }
    }
    else
    {
      beams.at(i)->set_delta(++a_delta);
    }
  }
  
  // reset gravity for next run
  if (collision)
  {
    gravity = true;
  }
  state_update();
}

void Game_State::key_pressed(unsigned char key, int x, int y)
{
  if (key >= 'A' && key <= 'Z')
  {
    key += 'a' - 'A';
  }
  switch (key)
  {
    if (!gravity)
    {
      case 'w':
        w = true;
        last_y = true;
        break;
      case 's':
        s = true;
        last_y = false;
        break;
    }
    case 'a':
      if (!(c != p &&
            ((Special*)c)->get_type() == KURT &&
            ((Kurt*)c)->get_ability()))
      {
        a = true;
        last_x = true;
      }
      break;
    case 'd':
      if (!(c != p &&
            ((Special*)c)->get_type() == KURT &&
            ((Kurt*)c)->get_ability()))
      {
        d = true;
        last_x = false;
      }
      break;
    case 'e':
      if (c != p)
      {
        ((Special *)c)->use_ability(map);
        if (((Special *)c)->get_type() == KURT && !((Kurt *)c)->get_ability())
        {
          moveables = ((Kurt *)c)->remove_blocks(moveables);
          map->set_moveables(moveables);
        }
      }
      break;
    case 'p':
      for (unsigned int i = 0; i < next_special && i < specials.size(); ++i)
      {
        specials.at(i)->use_ability(map);
        if (specials.at(i)->get_type() == KURT
            && !((Kurt *)specials.at(i))->get_ability())
        {
          moveables = ((Kurt *)specials.at(i))->remove_blocks(moveables);
          map->set_moveables(moveables);
        }
      }
      break;
    case 'g':
      if (c != p && ((Special *)c)->get_type() == JUMPER)
      {
        float lx, ly, cx, cy;
        c->get_top_left(cx, cy);
        if (c->will_collide_Dy(p, false))
        {
          p->get_top_left(lx, ly);
          p->move(cx - lx, cy - ly - TILE_HEIGHT);
        }
        for (unsigned int i = 0; i < specials.size(); ++i)
        {
          if (c == specials.at(i))
          {
            continue;
          }
          if (c->will_collide_Dy(specials.at(i), false))
          {
            specials.at(i)->get_top_left(lx, ly);
            specials.at(i)->move(cx - lx, cy - ly - TILE_HEIGHT);
          }
        }
      }
      break;
    case 'q':
      if (c != p)
      {
        if (((Special*)c)->is_following())
        {
          ((Special*)c)->stop_following();
          c->set_volume(UNFOCUSED_VOLUME);
        }
        else
        {
          ((Special*)c)->start_following();
          c->set_volume(MAX_VOLUME);
        }
      }
      else
      {
        p->switch_follow();
        for (unsigned int i = 0; i < specials.size(); ++i)
        {
          if (specials.at(i)->is_controllable())
          {
            if (p->should_follow())
            {
              specials.at(i)->start_following();
              specials.at(i)->set_volume(MAX_VOLUME);
            }
            else
            {
              specials.at(i)->stop_following();
              specials.at(i)->set_volume(UNFOCUSED_VOLUME);
            }
          }
        }
      }
      break;
    case '0':
      if (c == p)
      {
        break;
      }
      c = p;
      c->setVSpeed(0);
      c->setHSpeed(0);
      
      c->set_volume(MAX_VOLUME);
      for (unsigned int i = 0; i < specials.size(); ++i)
      {
        specials.at(i)->set_volume(MAX_VOLUME);
      }
      
      map_slide_effect = SLIDE_COUNTER;
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (last_key == key)
      {
        key_held = key;
        if ((unsigned int) (key - 49) < next_special &&
            (unsigned int) (key - 49) < specials.size() &&
            c != specials.at(key - 49))
        {
          c = specials.at(key - 49);
          c->setVSpeed(0);
          c->setHSpeed(0);
          
          p->set_volume(UNFOCUSED_VOLUME);
          for (unsigned int i = 0; i < specials.size(); ++i)
          {
            if (c == specials.at(i))
            {
              continue;
            }
            specials.at(i)->set_volume(UNFOCUSED_VOLUME);
          }
          
          map_slide_effect = SLIDE_COUNTER;
        }
      }
      break;
    case 'z':
      debug = !debug;
      break;
    case ' ':
      if (c != p && ((Special*)c)->get_type() == JUMPER)
      {
        ((Special*)c)->use_ability(map);
        break;
      }
      int old_speed, i;
      old_speed = c->getVSpeed();
      c->setVSpeed(GRAVITY_SPEED);
      for (i = 0; i < (int) specials.size(); ++i)
      {
        if ((Special*)this == specials.at(i))
        {
          break;
        }
      }
      if (i == (int) specials.size())
      {
        i = -1;
      }
      
      if (c->will_collide_y(map) ||
          c->will_collide_platform(map) ||
          c->will_collide_tile(map, LADDER, NULL) ||
          c->will_collide_moveables_y(moveables, -1, NULL) ||
          c->will_collide_specials_y(specials, i, NULL))
      {
        jump_delta = 0;
      }
      c->setVSpeed(old_speed);
      break;
  }
  last_key = key;
}

void Game_State::key_released(unsigned char key, int x, int y)
{
  if (key >= 'A' && key <= 'Z')
  {
    key += 'a' - 'A';
  }
  switch (key)
  {
    case 'w':
      c->setVSpeed(s?last_y=false,PLAYER_SPEED:0);
      w = false;
      break;
    case 'a':
      c->setHSpeed(d?last_x = false,PLAYER_SPEED:0);
      a = false;
      break;
    case 's':
      c->setVSpeed(w?last_y=true,-PLAYER_SPEED:0);
      s = false;
      break;
    case 'd':
      c->setHSpeed(a?last_x=true,-PLAYER_SPEED:0);
      d = false;
      break;
    // below is for debugging only
    // remove for final game version
    case 'h':
      collision = !collision;
      gravity = false;
      c->setVSpeed(0);
      p->setVSpeed(0);
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (key_held != key && key - 49 < (int) specials.size() &&
          c != specials.at(key - 49) &&
          specials.at(key - 49)->is_controllable())
      {
        specials.at(key - 49)->use_ability(map);
        if (specials.at(key - 49)->get_type() == KURT &&
            !((Kurt *)specials.at(key - 49))->get_ability())
        {
          moveables = ((Kurt *)specials.at(key - 49))->remove_blocks(moveables);
          map->set_moveables(moveables);
        }
        last_key = 0;
      }
      else
      {
        key_held = 0;
      }
      break;
  }
}

void Game_State::special_pressed(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      key_pressed('w', x, y);
      break;
    case GLUT_KEY_DOWN:
      key_pressed('s', x, y);
      break;
    case GLUT_KEY_RIGHT:
      key_pressed('d', x, y);
      break;
    case GLUT_KEY_LEFT:
      key_pressed('a', x, y);
      break;
  }
}

void Game_State::special_released(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      key_released('w', x, y);
      break;
    case GLUT_KEY_DOWN:
      key_released('s', x, y);
      break;
    case GLUT_KEY_RIGHT:
      key_released('d', x, y);
      break;
    case GLUT_KEY_LEFT:
      key_released('a', x, y);
      break;
  }
}

void Game_State::pause_volume(void)
{
  p->set_volume(PAUSE_VOLUME);
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->set_volume(PAUSE_VOLUME);
  }
}

void Game_State::unpause_volume(void)
{
  c->set_volume(MAX_VOLUME);
  if (c == p)
  {
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      specials.at(i)->set_volume(MAX_VOLUME);
    }
  }
  else
  {
    p->set_volume(UNFOCUSED_VOLUME);
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (c == specials.at(i))
      {
        continue;
      }
      specials.at(i)->set_volume(UNFOCUSED_VOLUME);
    }
  }
}

void Game_State::reset_keys(void)
{
  a = false;
  w = false;
  s = false;
  d = false;
  last_key = 0;
  key_held = 0;
}

void Game_State::clean(void)
{
  p->clean();
  delete p;
  map->clean();
  delete map;
  if (moveables.size() > 0)
  {
    moveables.at(0)->clean();
  }
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    delete moveables.at(i);
  }
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->clean();
    delete specials.at(i);
  }
  state_clean();
}

bool sort_by_height(Special *i, Special *j)
{
  return i->get_y() > j->get_y();
}
