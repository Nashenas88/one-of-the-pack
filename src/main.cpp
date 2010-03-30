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

State *s;
State *paused;
vector<State *> stack;

void initRendering(void);
void initSound(FMOD_SYSTEM **system);
void handleResize(int w, int h);
void handleKeypress(unsigned char key, int x, int y);
void handleKeyrelease(unsigned char key, int x, int y);
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
  
  /* Set handler functions */
  glutDisplayFunc(drawScene);
  glutKeyboardFunc(handleKeypress);
  glutKeyboardUpFunc(handleKeyrelease);
  glutReshapeFunc(handleResize);
  
  // objects that are needed by the state
  Player *p;
  Drawable *block, *background, *breakable, *plat, *ladder, *paused_background;
  Drawable *map_image, *pointer;
  Texture *t, *bg, *tiles, *pause_bg, *mi, *pi, *ahnold;
  Map *m;
  FMOD_SYSTEM *system;
  FMOD_SOUND *s_sound, *temp_sound;
  vector<FMOD_SOUND *> sounds;
  FMOD_CHANNEL *channel = 0;
  FMOD_RESULT result;
  
  int num_sounds;
  ifstream file;
  stringstream temp_string;
  
  // initializing the texture
  t = new Texture(RESOURCES LEVEL1 PLAYER_TEXTURE);
  bg = new Texture(RESOURCES LEVEL1 BACKGROUND_TEXTURE);
  tiles = new Texture(RESOURCES LEVEL1 TILE_TEXTURE);
  pause_bg = new Texture(RESOURCES PAUSED_BACKGROUND);
  mi = new Texture(RESOURCES LEVEL1 MAP1);
  pi = new Texture(RESOURCES POINTER_TEXTURE);
  ahnold = new Texture(RESOURCES LEVEL1 AHNOLD_TEXTURE);
  
  // initializing the sound system and the sounds
  initSound(&system);
  
  result = FMOD_System_CreateSound(system, RESOURCES LEVEL1 PLAYER_SOUND,
                                   FMOD_SOFTWARE, 0, &s_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(s_sound, FMOD_LOOP_NORMAL);
  ERRCHECK(result);
  
  file.open (RESOURCES LEVEL1 NUM_SOUNDS_FILE, ios::in);
  file >> num_sounds;
  file.close();
  
  for (int i = 1; i <= num_sounds; ++i)
  {
    temp_string << RESOURCES LEVEL1 << i << SOUND_FILE_TYPE;
    
    result = FMOD_System_CreateSound(system, temp_string.str().c_str(),
                                     FMOD_SOFTWARE, 0, &temp_sound);
    ERRCHECK(result);
    result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_NORMAL);
    ERRCHECK(result);
    
    sounds.push_back(temp_sound);
    temp_string.str("");
  }
  
  // initializing player and all other objects
  p = new Player(SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f,
                 SCREEN_HEIGHT / 2.0f - TILE_HEIGHT, PLAYER_RIGHT,
                 4, t, (direc) 2, false, 0, 0, system, s_sound, channel);
  block = new Drawable(0.0f, 0.0f, 1, 1, TILE, tiles);
  background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, bg);
  breakable = new Drawable(0.0f, 0.0f, 2, 1, TILE, tiles);
  plat = new Drawable(0.0f, 0.0f, 3, 1, TILE, tiles);
  ladder = new Drawable(0.0f, 0.0f, 4, 1, TILE, tiles);
  paused_background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, pause_bg);
  map_image = new Drawable(300, 300, 0, 0, TEXT, mi);
  pointer = new Drawable(650, 270, 0, 0, TEXT, pi);
  
  FMOD_DSP *pe;
  
  vector<Drawable*> v;
  v.clear();
  v.push_back(background);
  v.push_back(block);
  v.push_back(ladder);
  v.push_back(plat);
  v.push_back(breakable);
  v.push_back(plat);
  
  vector<Texture*> textures;
  textures.clear();
  textures.push_back(ahnold);
  
  vector<Drawable*> moveables;
  vector<Special *> specials;
  m = new Map(v);
  m->load_map((RESOURCES LEVEL1 MAP1), moveables, specials, textures, system,
              sounds, channel);
  s = new Game_State(p, m, moveables, specials, system);
  paused = new Pause_State(system, pe, (Game_State *)s, paused_background,
                           map_image, pointer);
  
  p->pause_sound();
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->pause_sound();
  }
  
  for (unsigned int i = 0; i < specials.size(); ++i)
  {
    specials.at(i)->setVSpeed(GRAVITY_SPEED);
  }
  
  // set timer function to update and call
  // it in 25 miliseconds
  srand((unsigned)time(NULL));
  glutTimerFunc(25, update, 0);
  
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
    case ' ':
      if (s == paused)
      {
        if (((Pause_State *)s)->get_selected() == 0 ||
            ((Pause_State *)s)->get_selected() == 1) // return
        {
          ((Pause_State *)s)->reset_selected();
          s = stack.at(stack.size() - 1);
          stack.pop_back();
        }
        else if (((Pause_State *)s)->get_selected() == 2) // quit
        {
          system_clean();
          exit(0);
        }
      }
      else
      {
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

// called ever 25 milliseconds to update the screen
void update(int delta)
{
  
  s->update(delta);
  
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
  
  s->draw();
  
  glutSwapBuffers();
}

// clean up all data
void system_clean(void)
{
  s->clean();
}
