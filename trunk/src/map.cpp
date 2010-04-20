#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

#include "ahnold.h"
#include "jumper.h"
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
  
  get_background()->draw();
  
  ((Drawable *)this)->get_top_left(x, y);
  
  for (int i = 0; i < get_width(); ++i)
  {
    for (int j = 0; j < get_height(); ++j)
    {
      if (map[i][j][M_TILE] == BG ||
          map[i][j][M_TILE] == OUTSIDE)
      {
        continue;
      }
      // move object to (0, 0) just in case
      tiles.at(map[i][j][M_TILE])->get_top_left(tx, ty);
      tiles.at(map[i][j][M_TILE])->move(-tx, -ty);
      // move to location to be drawn at
      tiles.at(map[i][j][M_TILE])->move(i * TILE_WIDTH + x, j * TILE_HEIGHT + y);
      tiles.at(map[i][j][M_TILE])->draw();
      // move back to (0, 0)
      tiles.at(map[i][j][M_TILE])->move(-i * TILE_WIDTH - x, -j * TILE_HEIGHT - y);
    }
  }
}

void Map::remove(int x, int y)
{
  map[x][y][M_TILE] = BG;
  map[x][y][M_COLL] = 0;
  // enter code to remove block from map
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

void Map::move_block(int x, int y)
{
  // enter code to move block 
}


bool Map::load_map(const char *map_bmp, vector<Moveable *> &moveables,
                   vector<Special *> &specials, Texture *tiles,
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
        map[x][y][M_TILE] = BLOCK;
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
                                      1, 5, AHNOLD_PUNCH_NUM, texs.at(AHNOLD),
                                      LEFT, system, musics.at(sound_num),
                                      m_channel, effects.at(AHNOLD),
                                      a_channel));
        ++sound_num;
      }
      // jumping helper
      else if(red[0] == 0 && green[0] == 255 && blue[0] == 0)
      {
        float mx, my;
        get_top_left(mx, my);
        specials.push_back(new Jumper(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                      1, 5, JUMPER_JUMP_NUM, texs.at(JUMPER),
                                      LEFT, system, musics.at(sound_num),
                                      m_channel, effects.at(JUMPER),
                                      a_channel));
        ++sound_num;
      }
      // moveable block
      else if(red[0] == 128 && green[0] == 0 && blue[0] == 0)
      {
        float mx, my;
        Moveable *move;
        get_top_left(mx, my);
        move = new Moveable(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                            BLOCKS, MOVEABLE_BLOCK, tiles, true);
        move->set_cur_frame(MOVEABLE_BLOCK);
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
      // moving platform
      else if(red[0] == 128 && green[0] == 0 && blue[0] == 128)
      {
        float mx, my;
        get_top_left(mx, my);
        moveables.push_back(new Moveable(x * TILE_WIDTH + mx, y * TILE_HEIGHT + my,
                                         BLOCKS, MOVEABLE_BLOCK, tiles, false));
      }
      // player start position
      else if(red[0] == 255 && green[0] == 0 && blue[0] == 255)
      {
        // move map so that player start is centered
        move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f - TILE_WIDTH,
             -y * TILE_WIDTH + SCREEN_HEIGHT / 2.0f - TILE_HEIGHT - y_offset);
        for (unsigned int i = 0; i < specials.size(); ++i)
        {
          specials.at(i)->move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f -
                               TILE_WIDTH, -y * TILE_WIDTH +
                               SCREEN_HEIGHT / 2.0f - TILE_HEIGHT - y_offset);
        }
        for (unsigned int i = 0; i < moveables.size(); ++i)
        {
          moveables.at(i)->move(-x * TILE_WIDTH + SCREEN_WIDTH / 2.0f -
                               TILE_WIDTH, -y * TILE_WIDTH +
                               SCREEN_HEIGHT / 2.0f - TILE_HEIGHT - y_offset);
        }
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
  
  int current, left, right, top;
  
  // now we dynamically change the wall tiles to walls and the corner tiles to corners
  for (unsigned int i = 1; i < width - 1; ++i)
  {
    for (unsigned int j = 1; j < height - 1; ++j)
    {
      current = map[i][j][M_TILE];
      left = map[i-1][j][M_TILE];
      right = map[i+1][j][M_TILE];
      top = map[i][j-1][M_TILE];
      
      if (current == BLOCK)
      {
        if (left == OUTSIDE)
        {
          map[i][j][M_TILE] = R_BLOCK;
        }
        else if (right == OUTSIDE)
        {
          map[i][j][M_TILE] = L_BLOCK;
        }
        else if (left < BLOCK ||
                 left > RC_BLOCK)
        {
          if (top < BLOCK ||
              top > RC_BLOCK)
          {
            map[i][j][M_TILE] = LC_BLOCK;
          }
          else if (top == LC_BLOCK ||
                   top == L_BLOCK)
          {
            map[i][j][M_TILE] = L_BLOCK;
          }
          else if (top == RC_BLOCK ||
                   top == R_BLOCK)
          {
            map[i][j][M_TILE] = R_BLOCK;
          }
        }
        else if (right < BLOCK ||
                 right > RC_BLOCK)
        {
          if(top < BLOCK ||
             top > RC_BLOCK)
          {
            map[i][j][M_TILE] = RC_BLOCK;
          }
          else if (top == RC_BLOCK ||
                   top == R_BLOCK)
          {
            map[i][j][M_TILE] = R_BLOCK;
          }
          else if (top == LC_BLOCK ||
                   top == L_BLOCK)
          {
            map[i][j][M_TILE] = L_BLOCK;
          }
        }
      }
    }
  }
  
  moves = moveables;
  specs = specials;
  
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
