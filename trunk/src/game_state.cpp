#include "math.h"
#include "game_state.h"
#include "jumper.h"

Game_State::Game_State(void)
:p(NULL), c(NULL), m(NULL), gravity(true), collision(true) {}

Game_State::Game_State(Player *pl, Map *map, vector<Drawable *> mvs,
                       vector<Special *> sps, FMOD_SYSTEM *system)
:State(system), p(pl), c(pl), m(map), moveables(mvs), specials(sps),
next_special(0), gravity(true), collision(true) {}

// draw static background, then map, then specials, then moveables, then player
void Game_State::draw(void)
{
  m->get_background()->draw();
  m->draw();
  for (unsigned int i = specials.size() - 1; i < specials.size() ; --i)
  {
    specials.at(i)->draw();
  }
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->draw();
  }
  p->draw();
}

// this is called every time the scene
// needs to be updated
void Game_State::update(int &delta)
{
  float x, y, map_x, map_y;
  int num_following = 0;
  c->get_top_left(x, y);
  m->get_top_left(map_x, map_y);

  bool control_movex = true, map_movex = false;
  bool control_movey = true, map_movey = false;
  bool player_movey = true;
  bool animate = false;
  bool link = false;
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    // if the player collides with the special, tell him to
    // stop following temporarily
    if (c == p && c->will_collide(specials.at(i)))
    {
      specials.at(i)->stop_following();
    }
    // if the special's a jumper, and it has a link
    // move the link the same as the jumper
    if (specials.at(i)->get_type() == JUMPER &&
        ((Jumper *)specials.at(i))->get_link() != NULL)
    {
      ((Jumper *)specials.at(i))->get_link()->setVSpeed(specials.at(i)->getVSpeed());
      ((Jumper *)specials.at(i))->get_link()->move(specials.at(i)->getHSpeed(), 0);
      if (((Jumper *)specials.at(i))->get_link() == p)
      {
        link = true;
      }
    }
  }

  // if we're not colliding with anything,
  // then FALL MWAHAHA - written on very
  // little sleep
  if (gravity)
  {
    // for the case of a special
    float map_y_val = map_y/TILE_HEIGHT;
    if (c != p)
    {
      float y_val = y/TILE_HEIGHT;
      if (y_val - map_y_val < m->get_height())
      {
        if (((Special*)c)->get_type() != JUMPER || !(c->getVSpeed() < 0))
        c->setVSpeed(GRAVITY_SPEED);
      }
    }
    
    // for the player, when he's not part of a link
    if (!link)
    {
      float px, py;
      p->get_top_left(px, py);
      float py_val = py / TILE_HEIGHT;
      if (py_val - map_y_val < m->get_height())
      {
        p->setVSpeed(GRAVITY_SPEED);
      }
    }
  }

  if (collision)
  {
    // if player collides with the goal, no need to
    // do anything else since we're done already
    if (p->will_collide_tile(m, GOAL, NULL))
    {
      delta = -1;
      return;
    }

    // if we are colliding with ladder, then turn off gravity
    if (gravity)
    {
      gravity = !p->will_collide_tile(m, LADDER, NULL);
      if (!gravity)
      {
        p->setVSpeed(0);
      }
    }
    else if (c == p)
    {
      gravity = !p->will_collide_tile(m, LADDER, NULL);
    }
    else
    {
      gravity = true;
    }

    // then check for collision
    control_movex = !c->will_collide_x(m);

    /* this is to deal with the case where the player has climbed the ladder
     * and then falls off. While both PLAYER_SPEED and GRAVITY_SPEED can
     * align properly by themselves, when used together, they can make it so
     * the player seems to "hover" above the floor. The following fixes this
     * hover issue.
     */
    control_movey = !c->will_collide_y(m) && !c->will_collide_platform(m) &&
              !c->will_collide_specials(specials);
    if (c != p)
    {
      player_movey = !p->will_collide_y(m) && !p->will_collide_platform(m) &&
                !p->will_collide_specials(specials);
      control_movey = control_movey && !c->will_collide_tile(m, LADDER, NULL) &&
                !c->will_collide_specials(specials);
    }
    if (c == p && !control_movey && c->getVSpeed() == GRAVITY_SPEED)
    {
      c->setVSpeed(PLAYER_SPEED);
      control_movey = !c->will_collide_y(m) && !c->will_collide_platform(m) &&
                !c->will_collide_specials(specials);
    }
    if (c != p && !player_movey && p->getVSpeed() == GRAVITY_SPEED)
    {
      p->setVSpeed(PLAYER_SPEED);
      player_movey = !p->will_collide_y(m) && !p->will_collide_platform(m) &&
                !p->will_collide_specials(specials);
    }
  }

  // map_xs and map_ys and the map movement speed in x and y
  float map_xs = c->getHSpeed(), map_ys = c->getVSpeed();

  // if we collide with the map movement boundary
  // then freeze the player and move the map
  if (control_movex &&
      ((c->getHSpeed() > 0 && x + c->get_width() > SCREEN_WIDTH - BORDER) ||
      (c->getHSpeed() < 0 && x < BORDER)))
  {
    control_movex = false;
    map_movex = true;
    map_xs = -map_xs;
  }
  if (control_movey &&
      ((c->getVSpeed() > 0 && y + c->get_height() > SCREEN_HEIGHT - BORDER) ||
      (c->getVSpeed() < 0 && y < BORDER)))
  {
    control_movey = false;
    map_movey = true;
    map_ys = -map_ys;
  }
  
  // move the map if necesary and also move the player
  if (map_movey && 0.0 >= map_y + map_ys &&
           map_y + map_ys + m->get_height() * TILE_HEIGHT >= SCREEN_HEIGHT)
  {
    m->move(0, map_ys);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      if (c == specials.at(i))
      {
        ((Drawable*)p)->move(0, map_ys);
        continue;
      }
      ((Drawable*)specials.at(i))->move(0, map_ys);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(0, map_ys);
    }
  }
  else if (control_movey)
  {
    c->move(0, c->getVSpeed());
  }

  if (c != p && player_movey)
  {
    p->move(0, p->getVSpeed());
  }

  if (map_movex && 0.0 >= map_x + map_xs &&
      map_x + map_xs + m->get_width() * TILE_WIDTH >= SCREEN_WIDTH)
  {
    animate = true;
    m->move(map_xs, 0);
    for (unsigned int i = 0; i < specials.size(); i++)
    {
      if (c == specials.at(i))
      {
        ((Drawable*)p)->move(map_xs, 0);
        continue;
      }
      ((Drawable*)specials.at(i))->move(map_xs, 0);
    }
    for (unsigned int i = 0; i < moveables.size(); i++)
    {
      moveables.at(i)->move(map_xs, 0);
    }
  }
  else if (control_movex && c->getHSpeed() != 0)
  {
    animate = true;
    c->move(c->getHSpeed(), 0);
  }

  // player animation
  if (animate && c->get_num_frames() > 1)
  {
    if (delta > DELTA_DELAY)
    {
      delta = 0;
      if ((c->get_animdir() == 1 &&
          c->get_cur_frame() == c->get_num_frames()) ||
          (c->get_animdir() == -1 && c->get_cur_frame() == 1))
      {
        c->set_animdir(c->get_animdir() * -1);
      }
      c->set_cur_frame(c->get_cur_frame() + c->get_animdir());
    }
    else
    {
      ++delta;
    }
  }
  else
  {
    delta = 0;
    c->set_cur_frame(1);
  }

  // specials gravity
  float dx, dy, dist;
  unsigned int cur_frame, a_delta;
  Special *s;
  if (collision)
  {
    for (unsigned int i = 0; i < specials.size(); ++i)
    {
      if(c == specials.at(i))
      {
        continue;
      }
      if (!(specials.at(i)->will_collide_y(m) ||
            specials.at(i)->will_collide_tile(m, LADDER, NULL) ||
            specials.at(i)->will_collide_platform(m)))
      {
        specials.at(i)->move(0, specials.at(i)->getVSpeed());
      }
    }
  }
  
  // special animations
  // for the jumper, when jumping and at the last animation
  // turn gravity back on
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    if (c == specials.at(i))
    {
      continue;
    }

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
          if (specials.at(i)->get_type() == JUMPER)
          {
            specials.at(i)->setVSpeed(GRAVITY_SPEED);
          }
        }
      }
      else
      {
        specials.at(i)->set_delta(++a_delta);
      }
    }
    
    // for following the player
    dx = (specials.at(i)->get_x() - p->get_x());
    dy = (specials.at(i)->get_y() - p->get_y());
    dist = sqrt((dx*dx)+(dy*dy));
    
    if (specials.at(i)->is_following())
    {
      num_following++;
      // alter distance based on number of specials following
      if (dist > TOO_CLOSE + (num_following-1)*TILE_WIDTH)
      {
        if (specials.at(i)->get_mute())
        {
          specials.at(i)->set_mute(false);
        }
        specials.at(i)->setHSpeed(!dx?0:(dx<0?PLAYER_SPEED:-PLAYER_SPEED));
        if (collision)
        {
          if (!(specials.at(i)->will_collide_x(m)))
          {
            specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
          }
        }
        else
        {
          specials.at(i)->move(specials.at(i)->getHSpeed(), 0);
        }
      }
    }
    else if (dist < FOLLOW_DIST && !specials.at(i)->is_following())
    {
      specials.at(i)->start_following();
      if (i > (unsigned int) next_special)
      {
        s = specials.at(i);
        specials.at(i) = specials.at(next_special);
        specials.at(next_special) = s;
        ++next_special;
      }
      else if (i == (unsigned int) next_special)
      {
        ++next_special;
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
        c->setVSpeed(-PLAYER_SPEED);
        break;
      case 's':
        c->setVSpeed(PLAYER_SPEED);
        break;
    }
    case 'a':
      c->setHSpeed(-PLAYER_SPEED);
      break;
    case 'd':
      c->setHSpeed(PLAYER_SPEED);
      break;
    case 'p':
      for (unsigned int i = 0; i < next_special && i < specials.size(); ++i)
      {
        specials.at(i)->set_tex_num(ABILITY);
        specials.at(i)->use_ability(m);
      }
      break;
    case 'g':
      if (c != p && ((Special *)c)->get_type() == JUMPER)
      {
        if (c->will_collide(p))
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
            if (c->will_collide(specials.at(i)))
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
        ((Special*)c)->set_mute(true);
        if (((Special *)c)->get_type() == JUMPER)
        {
          ((Jumper *)c)->set_link(NULL);
        }
      }
    case '0':
      c->setVSpeed(GRAVITY_SPEED);
      c = p;
      c->setVSpeed(0);
      c->setHSpeed(0);

      c->set_volume(MAX_VOLUME);
      for (unsigned int i = 0; i < specials.size(); ++i)
      {
        specials.at(i)->set_volume(MAX_VOLUME);
      }

      center();
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
      if ((unsigned int) (key - 49) < next_special &&
          (unsigned int) (key - 49) < specials.size())
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

        center();
      }
      break;
  }
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
      c->setVSpeed(0);
      break;
    case 'a':
      c->setHSpeed(0);
      break;
    case 's':
      c->setVSpeed(0);
      break;
    case 'd':
      c->setHSpeed(0);
      break;
    // below is for debugging only
    // remove for final game version
    case 'h':
      collision = !collision;
      gravity = false;
      c->setVSpeed(0);
      p->setVSpeed(0);
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

void Game_State::center(void)
{
  float x, y, map_x, map_y, move_x, move_y, map_width, map_height;

  c->get_top_left(x, y);
  m->get_top_left(map_x, map_y);
  move_x = SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f - x;
  move_y = SCREEN_HEIGHT / 2.0f - TILE_HEIGHT - y;
  map_width = m->get_width() * TILE_WIDTH;
  map_height = m->get_height() * TILE_HEIGHT;

  if (move_x + map_x > 0)
  {
    move_x = -map_x;
  }
  else if (move_x + map_x - SCREEN_WIDTH < -map_width)
  {
    move_x = map_width + map_x - SCREEN_WIDTH;
  }
  if (move_y + map_y > 0)
  {
    move_y = -map_y;
  }
  else if (move_y + map_y - SCREEN_HEIGHT < -map_height)
  {
    move_y = map_height + map_y - SCREEN_HEIGHT;
  }

  m->move(move_x,move_y);
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    ((Drawable *)specials.at(i))->move(move_x,move_y);
  }
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->move(move_x,move_y);
  }
  ((Drawable *)p)->move(move_x, move_y);
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
  m->clean();
  delete m;
  for (unsigned int i = 0; i < moveables.size(); ++i)
  {
    moveables.at(i)->clean();
    delete moveables.at(i);
  }
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->clean();
    delete specials.at(i);
  }
  state_clean();
}
