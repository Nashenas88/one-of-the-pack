#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>
using namespace std;

#include "ahnold.h"
#include "jumper.h"
#include "paris.h"
#include "engineer.h"
#include "map.h"
#include "defines.h"
#include "character.h"

Map::Map(void)
:Drawable(), map(NULL) {}

Map::Map(vector<Drawable *> t)
:Drawable(0.0f, 0.0f, 0, 0, TILE, (GLuint) 0), tiles(t) {}

// draws the map by going to each point and drawing
// the specified tile. ignore the tiles marked BG for
// background
void Map::draw(void)
{
  float x, y, tx, ty;
  Drawable *tile_to_draw;
  unsigned char self, above, top_left, top_right;
  
  get_background()->draw();
  
  ((Drawable *)this)->get_top_left(x, y);
  for (int i = 1; i < get_width() - 1; ++i)
  {
    for (int j = 1; j < get_height() - 1; ++j)
    {
      if (map[i][j][M_TILE] == BG ||
          map[i][j][M_TILE] == OUTSIDE)
      {
        continue;
      }
      
      tile_to_draw = tiles.at(map[i][j][M_TILE]);
      above = map[i][j - 1][M_TILE];
      top_left = map[i-1][j-1][M_TILE];
      top_right = map[i+1][j-1][M_TILE];
      self = map[i][j][M_TILE];
      
      // move object to (0, 0) just in case
      tile_to_draw->get_top_left(tx, ty);
      tile_to_draw->move(-tx, -ty);
      // move to location to be drawn at
      tile_to_draw->move(i * TILE_WIDTH + x, j * TILE_HEIGHT + y);
      // only draw if the object will be on screen
      if (tile_to_draw->get_x() > -TILE_WIDTH &&
          tile_to_draw->get_x() <= SCREEN_WIDTH &&
          tile_to_draw->get_y() > -TILE_HEIGHT &&
          tile_to_draw->get_y() <= SCREEN_HEIGHT)
      {
        if ((above == OUTSIDE && self >= BLOCK1 && self <= BLOCK5) ||
            ((top_left == OUTSIDE || top_right == OUTSIDE) &&
             (self == RC_BLOCK || self == LC_BLOCK)))
        {
          tile_to_draw->flip_y();
        }
        tile_to_draw->draw();
        if ((above == OUTSIDE && self >= BLOCK1 && self <= BLOCK5) ||
            ((top_left == OUTSIDE || top_right == OUTSIDE) &&
             (self == RC_BLOCK || self == LC_BLOCK)))
        {
          tile_to_draw->flip_y();
        }
      }
      // move back to (0, 0)
      tile_to_draw->move(-i * TILE_WIDTH - x, -j * TILE_HEIGHT - y);
    }
  }
}

void Map::remove(int x, int y)
{
  map[x][y][M_TILE] = BG;
  map[x][y][M_COLL] = 0;
  // enter code to remove block from map
}

bool Map::make_rubber(int x, int y)
{
  if (map[x][y][M_TILE] > BG && map[x][y][M_TILE] < RUBBER)
  {
    map[x][y][M_COLL] = map[x][y][M_TILE] + 1;
    map[x][y][M_TILE] = RUBBER;
    return true;
  }
  return false;
}

bool Map::return_from_rubber(int x, int y)
{
  if (map[x][y][M_TILE] == RUBBER && map[x][y][M_COLL] != 1)
  {
    map[x][y][M_TILE] = map[x][y][M_COLL] - 1;
    map[x][y][M_COLL] = 1;
    return true;
  }
  return false;
}

void Map::open_bouncer(int x, int y)
{
  map[x][y][M_TILE] = BOUNCER_OPEN;
  map[x][y][M_COLL] = 0;
}

void Map::calculate_location(Drawable *d, int &x, int &y)
{
  float lxm, tym;
  float lxo, tyo;
  
  get_top_left(lxm, tym);
  d->get_top_left(lxo, tyo);
  x = (int) (lxo - lxm) / TILE_WIDTH;
  y = (int) (tyo - tym) / TILE_HEIGHT;
}

void Map::move(float x, float y)
{
  Drawable *bg = tiles.at((int)BG);
  ((Drawable *)this)->move(x, y);
  bg->move(x / (get_width() * TILE_WIDTH) * (bg->get_width() - SCREEN_WIDTH * 2),
           y / (get_height() * TILE_HEIGHT) * (bg->get_height() - SCREEN_HEIGHT * 2));
}


bool Map::load_map(const char *map_bmp, vector<Moveable *> &moveables,
                   vector<Special *> &specials, Texture *ts,
                   vector<Texture*> texs, Character *player,
                   FMOD_SYSTEM *system, vector<FMOD_SOUND *> musics,
                   FMOD_CHANNEL *m_channel, vector<FMOD_SOUND *> effects,
                   FMOD_CHANNEL *a_channel)
{
  ifstream file;
  unsigned char red[1], green[1], blue[1], alpha[1];
  char buffer16[2];
  char buffer32[4];
  unsigned long offset, size, width, height;
  unsigned int bpp;
  
  file.open(map_bmp, ios::in | ios::binary);
  
  // Check for BM
  file.read(buffer16, 2);
  if(buffer16[0] != 'B' || buffer16[1] != 'M')
  {
    cout << "File " << map_bmp;
    cout << " is not a valid .bmp file: BM tag is missing" << endl;
    return false;
  }
  
  // skip file size and 4 unnecessary bytes
  file.seekg(8, ios_base::cur);
  
  // check for offset
  file.read(buffer32, 4);
  offset = *((unsigned long*)&buffer32);
  
  // check header size
  file.read(buffer32, 4);
  if (*((unsigned long*)&buffer32) != 40)
  {
    cout << "Cannot use " << map_bmp;
    cout << ": unsupported .bmp format" << endl;
    return false;
  }
  
  // read width
  file.read(buffer32, 4);
  width = *((unsigned long*)&buffer32);
  
  // read height
  file.read(buffer32, 4);
  height = *((unsigned long*)&buffer32);
  
  // check color plane
  file.read(buffer16, 2);
  memcpy(buffer32, buffer16, 2);
  buffer32[2] = buffer32[3] = 0;
  if(*((unsigned int*)&buffer32) != 1)
  {
    cout << "File " << map_bmp;
    cout << " is not a valid .bmp file: color plane not set to 1" << endl;
    return false;
  }
  
  // get bits per pixel
  file.read(buffer16, 2);
  memcpy(buffer32, buffer16, 2);
  buffer32[2] = buffer32[3] = 0;
  bpp = *((unsigned int*)&buffer32);
  if (bpp != 24 || bpp != 24)
  {
    cout << "Cannot use " << map_bmp;
    cout << "unsupported bits per pixel: " << bpp << endl;
  }
  
  // check compression
  file.read(buffer32, 4);
  if (*((unsigned long*)&buffer32) != 0)
  {
    cout << "Cannot use " << map_bmp;
    cout << "unsupported compression format" << endl;
    return false;
  }
  
  // get image size
  file.read(buffer32, 4);
  size = *((unsigned long*)&buffer32);
  
  // rest of header can be ignored so skip to offset
  // before reading bitmap data
  file.seekg(offset, ios_base::beg);
  
  // allocate map now that we have sizes
  map = (unsigned char ***)malloc(sizeof(unsigned char **) * width);
  for (unsigned int x = 0; x < width; ++x)
  {
    map[x] = (unsigned char **)malloc(sizeof(unsigned char *) * height);
    for (unsigned int y = 0; y < height; ++y)
    {
      map[x][y] = (unsigned char *)malloc(sizeof(unsigned char) * 2);
      map[x][y][M_TILE] = BG;
      map[x][y][M_COLL] = 0;
    }
  }
  
  srand (time(NULL));
  
  int sound_num;
  float y_offset;
  sound_num = 0;
  y_offset = player->get_col_y_offset();
  
  // start reading data from file
  for (int y = height - 1; y >= 0 && file.good(); --y)
  {
    for (int x = 0; x < (int)width && file.good(); ++x)
    { 
      file.read((char *)blue, 1);
      file.read((char *)green, 1);
      file.read((char *)red, 1);
      if (bpp == 32)
      {
        file.read((char *)alpha, 1);
      }
      
      // block
      if(red[0] == 0 && green[0] == 0 && blue[0] == 0)
      {
        map[x][y][M_COLL] = 1;
        if (x > 0)
        {
          do
          {
            map[x][y][M_TILE] = rand() % 5 + 1; // randomly selects a block 1-5
          } while (map[x-1][y][M_TILE] == map[x][y][M_TILE]);
        }
        else
        {
          map[x][y][M_TILE] = rand() % 5 + 1; // randomly selects a block 1-5
        }
      }
      // outside of the playable area
      if(red[0] == 0 && green[0] == 128 && blue[0] == 0)
      {
        map[x][y][M_COLL] = 1;
        map[x][y][M_TILE] = OUTSIDE;
      }
      // strong helper
      else if(red[0] == 255 && green[0] == 0 && blue[0] == 0)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Ahnold(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                      x, y, 1, 5, AHNOLD_PUNCH_NUM,
                                      texs.at(AHNOLD), LEFT,
                                      system, musics.at(sound_num), m_channel,
                                      effects.at(AHNOLD), a_channel));
        ++sound_num;
      }
      // jumping helper
      else if(red[0] == 0 && green[0] == 255 && blue[0] == 0)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Jumper(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                      x, y, 1, 5, JUMPER_JUMP_NUM,
                                      texs.at(JUMPER), LEFT, system,
                                      musics.at(sound_num), m_channel,
                                      effects.at(JUMPER), a_channel));
        ++sound_num;
      }
      // paris helper
      else if (red[0] == 255 && green[0] == 0 && blue[0] == 128)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Paris(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                     x, y, 1, 5, PARIS_KISS_NUM,
                                     texs.at(PARIS), LEFT, system,
                                     musics.at(sound_num), m_channel,
                                     effects.at(PARIS), a_channel));
        ++sound_num;
      }
      // engineer helper
      else if (red[0] == 255 && green[0] == 128 && blue[0] == 64)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Engineer(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                        x, y, 1, 5, ENGINEER_BUTTON_NUM,
                                        texs.at(ENGINEER), LEFT, system,
                                        musics.at(sound_num), m_channel,
                                        effects.at(ENGINEER), a_channel,
                                        effects.at(BEAM)));
        ++sound_num;
      }
      // Kurt helper
      else if (red[0] == 255 && green[0] == 128 && blue[0] == 192)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Kurt(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                    x, y, 1, 5, KURT_BUTTON_NUM,
                                    texas.at(KURT), LEFT, system,
                                    musics.at(sound_num), m_channel,
                                    effect.at(KURT), a_channel));
        ++sound_num;
      }
      // moveable block
      else if(red[0] == 128 && green[0] == 0 && blue[0] == 0)
      {
        float mx, my;
        Moveable *move;
        get_top_left(mx, my);
        move = new Moveable(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                            PUSH, 1, ts, true);
        moveables.push_back(move);
      }
      // ladder
      else if(red[0] == 128 && green[0] == 128 && blue[0] == 0)
      {
        map[x][y][M_TILE] = LADDER;
      }
      // platform (can jump from bottom)
      else if(red[0] == 128 && green[0] == 128 && blue[0] == 128)
      {
        map[x][y][M_TILE] = PLATFORM;
      }
      // breakeabe block
      else if(red[0] == 0 && green[0] == 128 && blue[0] == 255)
      {
        map[x][y][M_COLL] = 1;
        map[x][y][M_TILE] = BREAKABLE;
      }
      // goal
      else if(red[0] == 255 && green[0] == 255 && blue[0] == 0)
      {
        map[x][y][M_TILE] = GOAL;
      }
      // black hole tile
      else if(red[0] == 128 && green[0] == 128 && blue[0] == 255)
      {
        map[x][y][M_TILE] = BLACK_HOLE;
      }
      // moving platform
      else if(red[0] == 128 && green[0] == 0 && blue[0] == 128)
      {
        float mx, my;
        get_top_left(mx, my);
        moveables.push_back(new Moveable(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                         BLOCKS, 1, ts, false));
      }
      // player start position
      else if(red[0] == 255 && green[0] == 0 && blue[0] == 255)
      {
        // move map so that player start is centered
        move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f - TILE_WIDTH,
             -y * TILE_WIDTH + 3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT - y_offset);
        for (unsigned int i = 0; i < specials.size(); ++i)
        {
          specials.at(i)->move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f -
                               TILE_WIDTH, -y * TILE_WIDTH +
                               3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT - y_offset);
        }
        for (unsigned int i = 0; i < moveables.size(); ++i)
        {
          moveables.at(i)->move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f -
                               TILE_WIDTH, -y * TILE_WIDTH +
                               3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT - y_offset);
        }
      }
      // Body Guards
      // Right
      else if(red[0] == 0 && green[0] == 128 && blue[0] == 128)
      {
        map[x][y][M_COLL] = 1;
        map[x][y][M_TILE] = BOUNCER_CLOSED_R;
      }
      // Left
      else if(red[0] == 0 && green[0] == 128 && blue[0] == 129)
      {
        map[x][y][M_COLL] = 1;
        map[x][y][M_TILE] = BOUNCER_CLOSED_L;
      }
    }
    // apparently there's some sort of bitmap "newline"
    // in the bitmaps that ryan makes. this is to fix that issue
    if(width % 2 != 0)
    {
      file.read((char *)blue, 1);
    }
    /*file.read((char *)green, 1);
    file.read((char *)red, 1);
    if (bpp == 32)
    {
      file.read((char *)alpha, 1);
    }*/
    // commented out because this issue magically dissapeared
  }
  
  file.close();
  
  // don't forget to save the width and height
  set_width(width);
  set_height(height);
  
  int current, left, right, top, top_left, top_right, bottom;
  
  // now we dynamically change the wall tiles to walls and the corner tiles to corners
  for (unsigned int i = 1; i < width - 1; ++i)
  {
    for (unsigned int j = 1; j < height - 1; ++j)
    {
      current = map[i][j][M_TILE];
      left = map[i-1][j][M_TILE];
      right = map[i+1][j][M_TILE];
      top = map[i][j-1][M_TILE];
      top_left = map[i-1][j-1][M_TILE];
      top_right = map[i+1][j-1][M_TILE];
      bottom = map[i][j+1][M_TILE];
      
      if (current >= BLOCK1 && current <= BLOCK5)
      {
        if (left == OUTSIDE)
        {
          map[i][j][M_TILE] = R_BLOCK;
        }
        else if (right == OUTSIDE)
        {
          map[i][j][M_TILE] = L_BLOCK;
        }
        else if (left < BLOCK1 ||
                 left > RC_BLOCK)
        {
          if (top < BLOCK1 ||
              top > RC_BLOCK)
          {
            map[i][j][M_TILE] = LC_BLOCK;
          }
          else if (top == LC_BLOCK ||
                   top == L_BLOCK)
          {
            map[i][j][M_TILE] = L_BLOCK;
            if ((top_left == OUTSIDE && left >= BLOCK1 && left <= L_BLOCK) ||
                (top_right == OUTSIDE && right >= BLOCK1 && right <= L_BLOCK) &&
                bottom == BG)
            {
              map[i][j][M_TILE] = LC_BLOCK;
            }
          }
          else if (top == RC_BLOCK ||
                   top == R_BLOCK)
          {
            map[i][j][M_TILE] = R_BLOCK;
            if ((top_left == OUTSIDE && left >= BLOCK1 && left <= L_BLOCK) ||
                (top_right == OUTSIDE && right >= BLOCK1 && right <= L_BLOCK) &&
                bottom == BG)
            {
              map[i][j][M_TILE] = RC_BLOCK;
            }
          }
        }
        else if (right < BLOCK1 ||
                 right > RC_BLOCK)
        {
          if(top < BLOCK1 ||
             top > RC_BLOCK)
          {
            map[i][j][M_TILE] = RC_BLOCK;
          }
          else if (top == RC_BLOCK ||
                   top == R_BLOCK)
          {
            map[i][j][M_TILE] = R_BLOCK;
            if ((top_left == OUTSIDE && left >= BLOCK1 && left <= L_BLOCK) ||
                (top_right == OUTSIDE && right >= BLOCK1 && right <= L_BLOCK) &&
                bottom == BG)
            {
              map[i][j][M_TILE] = RC_BLOCK;
            }
          }
          else if (top == LC_BLOCK ||
                   top == L_BLOCK)
          {
            map[i][j][M_TILE] = L_BLOCK;
            if ((top_left == OUTSIDE && left >= BLOCK1 && left <= L_BLOCK) ||
                (top_right == OUTSIDE && right >= BLOCK1 && right <= L_BLOCK) &&
                bottom == BG)
            {
              map[i][j][M_TILE] = LC_BLOCK;
            }
          }
        }
      }
    }
  }
  
  moves = moveables;
  specs = specials;
  
  tiles.at((int)BG)->move(-SCREEN_WIDTH, -SCREEN_HEIGHT);
  
  return true;
}

void Map::clean(void)
{
  for (int i = 0; i < get_width(); ++i)
  {
    for (int j = 0; j < get_height(); ++j)
    {
      free (map[i][j]);
    }
    free (map[i]);
  }
  free (map);
  
  for (unsigned int i = 0; i < tiles.size(); ++i)
  {
    delete tiles.at(i);
  }
}
