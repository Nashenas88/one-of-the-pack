#include "math.h"
#include "game_state.h"
#include "jumper.h"

Game_State::Game_State(void)
:p(NULL), c(NULL), map(NULL), gravity(true), collision(true) {}

Game_State::Game_State(Player *pl, Map *m, vector<Moveable *> mvs,
                       vector<Special *> sps, vector<Drawable *> nums,
                       FMOD_SYSTEM *system)
:State(system), p(pl), c(pl), map(m), moveables(mvs), specials(sps),
numbers(nums), next_special(0), gravity(true), collision(true), w(0), a(0),
s(0), d(0), last_x(0), last_y(0), map_slide_effect(SLIDE_COUNTER), last_key(0),
key_held(0) {}

// draw static background, then map, then specials, then moveables, then player
void Game_State::draw(void)
{
  map->draw();
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->draw();
  }
  for (unsigned int i = specials.size() - 1; i < specials.size() ; --i)
  {
    specials.at(i)->draw();
  }
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
  
  if (last_x && a)
  {
    c->setHSpeed(-PLAYER_SPEED);
  }
  else if (d)
  {
    c->setHSpeed(PLAYER_SPEED);
  }
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
  
  // reset gravity for everything except the jumpers
  if (gravity)
  {
    p->setVSpeed(GRAVITY_SPEED);
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if (specials.at(i)->get_type() != JUMPER)
      {
        specials.at(i)->setVSpeed(GRAVITY_SPEED);
      }
      else if (specials.at(i)->get_tex_num() == SPECIAL)
      {
        specials.at(i)->setVSpeed(GRAVITY_SPEED);
      }
    }
  }
  
  bool follow_spec = false, follow_move = false;;
  int follow = -1;
  int mov_follow = -1;
  int temp_speed;
  Moveable *temp_move, *temp2;
  
  // check for collision on everyone
  // collision is always on for moveables
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    if (moveables.at(i)->get_gravity())
    {
      moveables.at(i)->setVSpeed(GRAVITY_SPEED);
    }
    if(moveables.at(i)->will_collide_y(map))
    {
      moveables.at(i)->setVSpeed(0);
    }
    moveables.at(i)->move(0, moveables.at(i)->getVSpeed());
    
    if(moveables.at(i)->will_collide_x(map))
    {
      moveables.at(i)->setHSpeed(0);
      temp_move = moveables.at(i)->get_link();
      while(temp_move)
      {
        temp_move->setHSpeed(0);
        temp2 = temp_move;
        temp_move = temp_move->get_link();
        temp2->set_link(NULL);
        if (temp_move == moveables.at(i))
        {
          break;
        }
      }
    }
    
    if(moveables.at(i)->will_collide_moveables_x(moveables, i, &mov_follow))
    {
      moveables.at(mov_follow)->setHSpeed(moveables.at(i)->getHSpeed());
      moveables.at(mov_follow)->set_link(moveables.at(i));
    }
  }
  if (collision)
  {
    // check vertical collision for specials
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
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
        }
        continue;
      }
      
      follow_move = specials.at(i)->will_collide_moveables_y(moveables, -1, &mov_follow);
      follow_spec = specials.at(i)->will_collide_specials_y(specials, i, &follow);
      
      // if on special or moveable, move with them
      if (follow_spec && follow >= 0 && !p->will_collide_Dx(specials.at(follow)))
      {
        specials.at(i)->setHSpeed(specials.at(i)->getHSpeed() +
                                  specials.at(follow)->getHSpeed());
        specials.at(i)->setVSpeed(specials.at(follow)->getVSpeed());
      }
      else if (follow_move && mov_follow >= 0)
      {
        specials.at(i)->setHSpeed(specials.at(i)->getHSpeed() +
                                  moveables.at(mov_follow)->getHSpeed());
        specials.at(i)->setVSpeed(moveables.at(mov_follow)->getVSpeed());
      }
      
      // move specials in the y if they can
      if (!(specials.at(i)->will_collide_y(map) ||
            specials.at(i)->will_collide_tile(map, LADDER, NULL) ||
            specials.at(i)->will_collide_platform(map)// ||
            /*specials.at(i)->will_collide_specials_y(specials, i, NULL) ||
            specials.at(i)->will_collide_moveables_y(moveables, -1, NULL)*/))
      {
        specials.at(i)->move(0, specials.at(i)->getVSpeed());
      }
      else
      {
        specials.at(i)->setVSpeed(0);
      }
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
    p->setVSpeed(temp_speed);
    
    // if on special or moveable, move with them
    if (follow_spec && follow >= 0 && !p->will_collide_Dx(specials.at(follow)))
    {
      p->setHSpeed(p->getHSpeed() + specials.at(follow)->getHSpeed());
      p->setVSpeed(specials.at(follow)->getVSpeed());
    }
    else if (follow_move && mov_follow >= 0)
    {
      p->setHSpeed(p->getHSpeed() + moveables.at(mov_follow)->getHSpeed());
      p->setVSpeed(moveables.at(mov_follow)->getVSpeed());
    }
    
    player_movey = !p->will_collide_y(map) && !p->will_collide_platform(map);
    
    if (!player_movey && !follow_spec && !follow_move &&
        p->getVSpeed() == GRAVITY_SPEED)
    {
      p->setVSpeed(PLAYER_SPEED);
      // check here to see if we'll follow a special
      follow_spec = p->will_collide_specials_y(specials, -1, &follow);
      follow_move = p->will_collide_moveables_y(moveables, -1, &mov_follow);
      
      // if on special or moveable, move with them
      if (follow_spec && follow >= 0 && !p->will_collide_Dx(specials.at(follow)))
      {
        p->setHSpeed(p->getHSpeed() + specials.at(follow)->getHSpeed());
        p->setVSpeed(specials.at(follow)->getVSpeed());
      }
      else if (follow_move && mov_follow >= 0)
      {
        p->setHSpeed(p->getHSpeed() + moveables.at(mov_follow)->getHSpeed());
        p->setVSpeed(moveables.at(mov_follow)->getVSpeed());
      }
      player_movey = !p->will_collide_y(map) && !p->will_collide_platform(map) &&
                     !p->will_collide_tile(map, LADDER, NULL);
    }
    if (!player_movey)
    {
      p->setVSpeed(0);
    }
    
    if (c != p)
    {
      p->setHSpeed(p->will_collide_x(map) ||
                   p->will_collide_moveables_x(moveables, -1, NULL)?
                   0 : p->getHSpeed());
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
      }
      else
      {
        p->setVSpeed(temp);
      }
      p->move(0, -temp);
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
      ++num_following;
      
      // if the special is not too close and it's not colliding with anything
      // then change its speed
      if (dist > TOO_CLOSE + (num_following-1)*TILE_WIDTH && follow != (int)i)
      {
        if (!specials.at(i)->will_collide_x(map) &&
            !specials.at(i)->will_collide_moveables_x(moveables, -1, NULL))
        {
          specials.at(i)->setHSpeed(!dx?0:(dx<0?PLAYER_SPEED:-PLAYER_SPEED));
        }
        else
        {
          specials.at(i)->setHSpeed(0);
        }
      }
      // if it's too close, stop it from moving
      else
      {
        specials.at(i)->setHSpeed(0);
      }
    }
    // if it isn't following and it's close enough, then make it start
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
  
  // move specials in the x
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    // if this special is being controlled, then don't move it. this will
    // be done when we check for collision with the map
    if (c == specials.at(i))
    {
      continue;
    }
    specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
  }
  
  // move moveables
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->move(moveables.at(i)->getHSpeed(), 0);
  }
  
  // move map if controlled character is at edge of map boundary
  
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
    int mx, my;
    
    c->get_top_left(control_x, control_y);
    center_x = SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f;
    center_y = SCREEN_HEIGHT / 2.0f - TILE_HEIGHT;
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
        moveables.at(i)->move(mx, my);
      }
    }
  }
  
  // animation for plyer
  if (c == p && p->get_num_frames() > 1 && p->getHSpeed() != 0)
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
  
  // animation for specials
  // for the jumper, when jumping and at the last animation
  // turn gravity back on
  unsigned int a_delta, cur_frame;
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    a_delta = specials.at(i)->get_delta();
    if (specials.at(i)->get_tex_num() == ABILITY)
    {
      if (a_delta > SPECIAL_DELTA_DELAY)
      {
        specials.at(i)->set_delta(0);
        cur_frame = specials.at(i)->get_cur_frame();
        if (cur_frame < specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(cur_frame + 1);
        }
        else if (cur_frame == specials.at(i)->get_abil_frames())
        {
          specials.at(i)->set_cur_frame(1);
          specials.at(i)->set_tex_num(SPECIAL);
        }
      }
      else
      {
        specials.at(i)->set_delta(++a_delta);
      }
    }
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
      a = true;
      last_x = true;
      break;
    case 'd':
      d = true;
      last_x = false;
      break;
    case 'e':
      if (c != p)
      {
        ((Special *)c)->use_ability(map);
      }
      break;
    case 'p':
      for (unsigned int i = 0; i < next_special && i < specials.size(); ++i)
      {
        specials.at(i)->use_ability(map);
      }
      break;
    case 'g':
      if (c != p && ((Special *)c)->get_type() == JUMPER)
      {
        if (c->will_collide_Dy(p))
        {
          ((Jumper *)c)->set_link(p);
        }
        else
        {
          for (unsigned int i = 0; i < specials.size(); ++i)
          {
            if (c == specials.at(i))
            {
              continue;
            }
            if (c->will_collide_Dy(specials.at(i)))
            {
              ((Jumper *)c)->set_link(specials.at(i));
              break;
            }
          }
        }
        if (((Jumper *)c)->get_link() != NULL)
        {
          float lx, ly, cx, cy;
          c->get_top_left(cx, cy);
          ((Jumper *)c)->get_link()->get_top_left(lx, ly);
          ((Drawable *)((Jumper *)c)->get_link())->move(cx - lx, cy - ly - 80);
          ((Jumper *)c)->get_link()->setVSpeed(0);
        }
      }
      break;
    case 'q':
      if (c != p)
      {
        ((Special*)c)->stop_following();
        //((Special*)c)->set_mute(true);
        if (((Special *)c)->get_type() == JUMPER)
        {
          ((Jumper *)c)->set_link(NULL);
        }
      }
    case '0':
      if (c == p)
      {
        break;
      }
      c->setVSpeed(GRAVITY_SPEED);
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
      if (key_held != key && c != specials.at(key - 49))
      {
        specials.at(key - 49)->use_ability(map);
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
