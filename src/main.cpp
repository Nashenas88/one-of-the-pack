#include <stdlib.h>
#include <time.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include "player.h"
#include "map.h"
#include "state.h"
#include "game_state.h"
#include "pause_state.h"
#include "moveable.h"

State *s;
State *paused;
vector<State *> stack;
bool loading;
Drawable *load_screen;
int level;

void initRendering(void);
void initSound(FMOD_SYSTEM **system);
void initLoading(void);
void initLevel(int level);
void handleResize(int w, int h);
void handleKeypress(unsigned char key, int x, int y);
void handleKeyrelease(unsigned char key, int x, int y);
void handleSpecialpress(int key, int x, int y);
void handleSpecialrelease(int key, int x, int y);
void update(int value);
void drawScene(void);
void system_clean(void);

int main(int argc, char *argv[])
{
  /* Initialize glut */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  /* create window */
  glutCreateWindow(GAME_NAME);
  initRendering();
  initLoading();
  
  level = 1;
  
  /* Set handler functions */
  glutDisplayFunc(drawScene);
  glutKeyboardFunc(handleKeypress);
  glutKeyboardUpFunc(handleKeyrelease);
  glutSpecialFunc(handleSpecialpress);
  glutSpecialUpFunc(handleSpecialrelease);
  glutReshapeFunc(handleResize);
  
  // set timer function to update and call
  // it in 25 miliseconds
  srand((unsigned)time(NULL));
  glutTimerFunc(25, update, 0);
  glutTimerFunc(25, initLevel, level);
  
  glutMainLoop();
  
  return 0;
}

//Initializes 3D rendering
void initRendering(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
}

// Initializes sound playback
void initSound(FMOD_SYSTEM **system)
{
  FMOD_RESULT result;
  unsigned int version;
  
  result = FMOD_System_Create(system);
  ERRCHECK(result);
  
  result = FMOD_System_GetVersion(*system, &version);
  ERRCHECK(result);
  
  if (version < FMOD_VERSION)
  {
    printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    exit(0);
  }
  
  result = FMOD_System_Init(*system, 32, FMOD_INIT_NORMAL, 0);
  ERRCHECK(result);
}

void initLoading(void)
{
  Texture *tex;
  stringstream ss;
  
  loading = true;
  ss << RESOURCES << LOADING_TEXTURE;
  tex = new Texture(ss.str().c_str());
  load_screen = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, tex);
}

void initLevel(int level)
{
    // objects that are needed by the state
  Player *p;
  Drawable *block, *background, *breakable, *plat, *ladder, *paused_background;
  Drawable *map_image, *pointer, *left_block, *right_block, *left_corner_block;
  Drawable *right_corner_block, *moveable, *goal;
  Texture *t, *bg, *tiles, *pause_bg, *mi, *pi, *ahnold, *jumper;
  vector<Texture*> textures;
  vector<Moveable*> moveables;
  vector<Special*> specials;
  Map *m;
  FMOD_SYSTEM *system;
  FMOD_SOUND *s_sound, *temp_sound;
  vector<FMOD_SOUND *> musics, effects;
  FMOD_CHANNEL *m_channel = 0, *a_channel = 0;
  FMOD_RESULT result;
  FMOD_DSP *pe;
  
  int num_sounds;
  ifstream file;
  stringstream temp_string;
  
  
  // initializing the texture
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << PLAYER_TEXTURE;
  t = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << BACKGROUND_TEXTURE;
  bg = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << TILE_TEXTURE;
  tiles = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << PAUSED_BACKGROUND;
  pause_bg = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << MAP_IMAGE;
  mi = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << POINTER_TEXTURE;
  pi = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << AHNOLD_TEXTURE;
  ahnold = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << JUMPER_TEXTURE;
  jumper = new Texture(temp_string.str().c_str());
  
  // initializing the sound system and the sounds
  initSound(&system);
  
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << PLAYER_SOUND;
  result = FMOD_System_CreateSound(system, temp_string.str().c_str(),
                                   FMOD_SOFTWARE, 0, &s_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(s_sound, FMOD_LOOP_NORMAL);
  ERRCHECK(result);
  
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << NUM_SOUNDS_FILE;
  file.open (temp_string.str().c_str(), ios::in);
  file >> num_sounds;
  file.close();
  
  for (int i = 1; i <= num_sounds; ++i)
  {
    temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << i << SOUND_FILE_TYPE;
    result = FMOD_System_CreateSound(system, temp_string.str().c_str(),
                                     FMOD_SOFTWARE, 0, &temp_sound);
    ERRCHECK(result);
    result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_NORMAL);
    ERRCHECK(result);
    
    musics.push_back(temp_sound);
  }
  result = FMOD_System_CreateSound(system, RESOURCES AHNOLD_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(system, RESOURCES JUMPER_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  
  // initializing player and all other objects
  p = new Player(SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f,
                 SCREEN_HEIGHT / 2.0f - TILE_HEIGHT, PLAYER_RIGHT,
                 5, t, RIGHT, false, system, s_sound, m_channel);
  block = new Drawable(0.0f, 0.0f, WALLS, 1, TILE, tiles);
  left_block = new Drawable(0.0f, 0.0f, WALLS, 1, TILE, tiles);
  left_block->set_cur_frame(LEFT_WALL_FRAME);
  right_block = new Drawable(0.0f, 0.0f, WALLS, 1, TILE, tiles);
  right_block->set_cur_frame(RIGHT_WALL_FRAME);
  left_corner_block = new Drawable(0.0f, 0.0f, WALLS, 1, TILE, tiles);
  left_corner_block->set_cur_frame(LEFT_CORNER_WALL_FRAME);
  right_corner_block = new Drawable(0.0f, 0.0f, WALLS, 1, TILE, tiles);
  right_corner_block->set_cur_frame(RIGHT_CORNER_WALL_FRAME);
  background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, bg);
  breakable = new Drawable(0.0f, 0.0f, BLOCKS, 1, TILE, tiles);
  moveable = new Drawable(0.0f, 0.0f, BLOCK, 1, TILE, tiles);
  moveable->set_cur_frame(MOVEABLE_BLOCK);
  plat = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  ladder = new Drawable(0.0f, 0.0f, LADDS, 1, TILE, tiles);
  paused_background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, pause_bg);
  map_image = new Drawable(300, 300, 1, 1, VARIABLE, mi);
  pointer = new Drawable(650, 270, 1, 1, VARIABLE, pi);
  goal = new Drawable(0.0f, 0.0f, GOALT, 1, TILE, tiles);
  
  // place all drawables into vector
  // this order MUST match the order
  // of the tile_type enum
  vector<Drawable*> v;
  v.clear();
  v.push_back(background);
  v.push_back(block);
  v.push_back(left_block);
  v.push_back(right_block);
  v.push_back(left_corner_block);
  v.push_back(right_corner_block);
  v.push_back(ladder);
  v.push_back(plat);
  v.push_back(breakable);
  v.push_back(goal);
  
  textures.clear();
  textures.push_back(ahnold);
  textures.push_back(jumper);
  
  m = new Map(v);
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << MAP1;
  m->load_map(temp_string.str().c_str(), moveables, specials, tiles, textures,
              p, system, musics, m_channel, effects, a_channel);
  s = new Game_State(p, m, moveables, specials, system);
  paused = new Pause_State(system, pe, (Game_State *)s, paused_background,
                           map_image, pointer);
  
  // this is for syncing the music in the game
  p->pause_sound();
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->pause_sound();
  }
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->setVSpeed(GRAVITY_SPEED);
  }
  
  loading = false;
}

//Called when the window is resized
void handleResize(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) SCREEN_WIDTH, (GLdouble) SCREEN_HEIGHT, 0.0f);
}

// called when a normal key is pressed
void handleKeypress(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 27: // escape key
      system_clean();
      exit(0);
      break;
    case '\n':
    case '\r':
    case ' ':
      if (s == paused)
      {
        if (((Pause_State *)s)->get_selected() == 0) // return
        {
          ((Pause_State *)s)->reset_selected();
          s = stack.at(stack.size() - 1);
          ((Game_State *)s)->unpause_volume();
          stack.pop_back();
        }
        else if (((Pause_State *)s)->get_selected() == 1) // restart
        {
          loading = true;
          system_clean();
          glutTimerFunc(25, initLevel, level);
        }
        else if (((Pause_State *)s)->get_selected() == 2) // quit
        {
          system_clean();
          exit(0);
        }
      }
      else
      {
        ((Game_State *)s)->pause_volume();
        stack.push_back(s);
        s = paused;
      }
      return;
  }
  s->key_pressed(key, x, y);
}

void handleKeyrelease(unsigned char key, int x, int y)
{
  s->key_released(key, x, y);
}

void handleSpecialpress(int key, int x, int y)
{
  s->special_pressed(key, x, y);
}

void handleSpecialrelease(int key, int x, int y)
{
  s->special_released(key, x, y);
}

// called ever 25 milliseconds to update the screen
void update(int delta)
{
  if (!loading)
  {
    s->update(delta);
    // this means the goal state has been reached
    if (delta < 0)
    {
      loading = true;
      system_clean();
      if (++level <= LAST_LEVEL)
      {
        glutTimerFunc(25, initLevel, level);
      }
      else
      {
        printf("You win!\n");
        exit(0);
      }
    }
  }
  
  /* Tell GLUT that the display has changed */
  glutPostRedisplay();
  
  /* Tell GLUT to call update again in 25 milliseconds */
  glutTimerFunc(25, update, delta);
}

// draws everything that needs to be drawn
void drawScene(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
  glLoadIdentity(); //Reset the drawing perspective
  
  if (loading)
  {
    load_screen->draw();
  }
  else
  {
    s->draw();
  }
  
  glutSwapBuffers();
}

// clean up all data
void system_clean(void)
{
  paused->clean();
  delete paused;
}
