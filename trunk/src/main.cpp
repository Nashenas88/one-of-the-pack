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
#include "main_menu_state.h"
#include "tutorial_state.h"
#include "moveable.h"

State *s;
State *paused;
State *main_s;
State *tutorial;
vector<State *> stack;
vector<FMOD_SOUND *> sounds;
vector<Texture *> textures_tr;
bool loading;
bool control;
Drawable *load_screen;
Drawable *control_screen;
int level;
int last_level;
FMOD_SYSTEM *sound_system;

void initRendering(void);
void initSound(FMOD_SYSTEM **system);
void initLoading(void);
void initMain(int blah);
void initTutorial(int blah);
void initLevel(int level);
void initMaxLevel(void);
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
  level = 0; // signifies main menu
  initMaxLevel();
  
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
  glutTimerFunc(25, initTutorial, 0);
  
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
  control = false;
  ss << RESOURCES << LOADING_TEXTURE;
  tex = new Texture(ss.str().c_str());
  load_screen = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, tex);
  
  ss.str("");
  ss << RESOURCES << CONTROL_TEXTURE;
  tex = new Texture(ss.str().c_str());
  control_screen = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, tex);
}

void initMain(int blah)
{
  FMOD_SOUND *sound;
  FMOD_CHANNEL *channel;
  FMOD_RESULT result;
  
  Drawable *background, *pointer;
  Texture *main_menu, *pointer_tex;
  
  stringstream temp_str;
  
  temp_str.str(""); temp_str << RESOURCES << MAIN_MENU_TEXTURE;
  main_menu = new Texture(temp_str.str().c_str());
  temp_str.str(""); temp_str << RESOURCES << MAIN_MENU_POINTER_TEXTURE;
  pointer_tex = new Texture(temp_str.str().c_str());
  
  background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, main_menu);
  pointer = new Drawable(0.0f, 0.0f, 1, 1, VARIABLE, pointer_tex);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES MAIN_MENU_MUSIC,
                                   FMOD_SOFTWARE, 0, &sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(sound, FMOD_LOOP_NORMAL);
  ERRCHECK(result);
  
  
  main_s = new Main_Menu_State(background, pointer, sound_system, sound,
                               channel);
  ((Main_Menu_State*)main_s)->play_sound();
  
  s = main_s;
  loading = false;
}

void initTutorial(int blah)
{
  FMOD_SOUND *sound;
  FMOD_CHANNEL *channel;
  FMOD_RESULT result;
  
  // initializing the sound system and the sounds
  initSound(&sound_system);
  
  vector<Drawable *> slides;
  vector<Texture *> textures;
  unsigned int num_slides;
  
  stringstream temp_str;
  
  ifstream fin;
  fin.open(RESOURCES TUTORIAL NUM_TUTORIALS);
  fin >> num_slides;
  fin.close();
  
  for (unsigned int i = 0; i < num_slides; ++i)
  {
    temp_str.str(""); temp_str << RESOURCES TUTORIAL << i << ".png";
    textures.push_back (new Texture(temp_str.str().c_str()));
  }
  for (unsigned int i = 0; i < num_slides; ++i)
  {
    slides.push_back(new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, textures.at(i)));
  }
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES TUTORIAL TUTORIAL_MUSIC,
                                   FMOD_SOFTWARE, 0, &sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(sound, FMOD_LOOP_NORMAL);
  ERRCHECK(result);
  
  
  tutorial = new Tutorial_State(slides, sound_system, sound, channel);
  
  s = tutorial;
  loading = false;
}

void initMaxLevel(void)
{
  ifstream fin;
  fin.open(RESOURCES LAST_LEVEL_FILE);
  fin >> last_level;
  fin.close();
}

void initLevel(int level)
{
  // objects that are needed by the state
  Player *p;
  Drawable *block1, *background, *breakable1, *plat1, *ladder, *paused_background;
  Drawable *map_image, *pointer, *left_block, *right_block, *left_corner_block;
  Drawable *right_corner_block, *moveable, *goal, *block2, *block3, *block4;
  Drawable *block5, *black_hole, *bouncer_cr, *bouncer_cl, *bouncer_o, *rubber;
  Drawable *circuit, *n_check, *o_check, *toxic_sludge, *paused_names, *smog;
  Drawable *br_bl, *bl_bl, *plat2, *plat3, *plat4, *plat5, *breakable2, *breakable3;
  Texture *t, *bg, *tiles, *pause_bg, *mi, *pi, *ahnold, *jumper, *nums, *ps_ic;
  Texture *engineer, *paris, *kurt, *p_names;
  vector<Texture*> textures;
  vector<Moveable*> moveables;
  vector<Special*> specials;
  vector<Drawable*> numbers;
  Map *m;
  FMOD_SOUND *s_sound, *temp_sound;
  vector<FMOD_SOUND *> musics, effects, collects;
  FMOD_CHANNEL *m_channel = 0, *a_channel = 0;
  FMOD_RESULT result;
  
  int num_sounds;
  ifstream file;
  stringstream temp_string;
  
  
  // initializing the texture
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << PLAYER_TEXTURE;
  t = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << BACKGROUND_TEXTURE;
  bg = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << TILE_TEXTURE;
  tiles = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << PAUSED_BACKGROUND;
  pause_bg = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << MAP_IMAGE;
  mi = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << POINTER_TEXTURE;
  pi = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << AHNOLD_TEXTURE;
  ahnold = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << JUMPER_TEXTURE;
  jumper = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << ENGINEER_TEXTURE;
  engineer = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << PARIS_TEXTURE;
  paris = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES /*<< LEVEL << level << "/"*/ << KURT_TEXTURE;
  kurt = new Texture(temp_string.str().c_str());
  temp_string.str(""); temp_string << RESOURCES << PAUSED_NAMES_TEXTURE;
  p_names = new Texture(temp_string.str().c_str());
  
  temp_string.str(""); temp_string << RESOURCES << NUMBER_TEXTURE;
  nums = new Texture(temp_string.str().c_str());
  for (unsigned int i = 1; i < 10; ++i)
  {
    numbers.push_back(new Drawable(0.0f, 0.0f, 1, 1, NUM, nums));
    numbers.at(i-1)->set_cur_frame(i);
  }
  
  temp_string.str(""); temp_string << RESOURCES << PAUSE_ICONS_TEXTURE;
  ps_ic = new Texture(temp_string.str().c_str());
  
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << PLAYER_SOUND;
  result = FMOD_System_CreateSound(sound_system, temp_string.str().c_str(),
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
    result = FMOD_System_CreateSound(sound_system, temp_string.str().c_str(),
                                     FMOD_SOFTWARE, 0, &temp_sound);
    ERRCHECK(result);
    result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_NORMAL);
    ERRCHECK(result);
    
    musics.push_back(temp_sound);
  }
  result = FMOD_System_CreateSound(sound_system, RESOURCES AHNOLD_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES JUMPER_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES ENGINEER_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES KURT_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES PARIS_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES BEAM_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES JUMPER_SFX, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  effects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES AHNOLD_COLLECT, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  collects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES JUMPER_COLLECT, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  collects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES ENGINEER_COLLECT, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  collects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES KURT_COLLECT, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  collects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  result = FMOD_System_CreateSound(sound_system, RESOURCES PARIS_COLLECT, FMOD_SOFTWARE,
                                   0, &temp_sound);
  ERRCHECK(result);
  result = FMOD_Sound_SetMode(temp_sound, FMOD_LOOP_OFF);
  ERRCHECK(result);
  collects.push_back(temp_sound);
  sounds.push_back(temp_sound);
  
  // initializing player and all other objects
  p = new Player(SCREEN_WIDTH / 2.0f - TILE_WIDTH / 2.0f,
                 3.0f * SCREEN_HEIGHT / 4.0f - TILE_HEIGHT, PLAYER_RIGHT,
                 5, t, RIGHT, false, sound_system, s_sound, m_channel);
  block1 = new Drawable(0.0f, 0.0f, FLOORS, 1, TILE, tiles);
  block2 = new Drawable(0.0f, 0.0f, FLOORS, 1, TILE, tiles);
  block2->set_cur_frame(2);
  block3 = new Drawable(0.0f, 0.0f, FLOORS, 1, TILE, tiles);
  block3->set_cur_frame(3);
  block4 = new Drawable(0.0f, 0.0f, FLOORS, 1, TILE, tiles);
  block4->set_cur_frame(4);
  block5 = new Drawable(0.0f, 0.0f, FLOORS, 1, TILE, tiles);
  block5->set_cur_frame(5);
  left_block = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  left_block->set_cur_frame(LEFT_WALL_FRAME);
  right_block = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  right_block->set_cur_frame(RIGHT_WALL_FRAME);
  left_corner_block = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  left_corner_block->set_cur_frame(LEFT_CORNER_WALL_FRAME);
  right_corner_block = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  right_corner_block->set_cur_frame(RIGHT_CORNER_WALL_FRAME);
  br_bl = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  br_bl->set_cur_frame(BR_CORNER_WALL_FRAME);
  bl_bl = new Drawable(0.0f, 0.0f, SIDES_CORNERS, 1, TILE, tiles);
  bl_bl->set_cur_frame(BR_CORNER_WALL_FRAME);
  bl_bl->change_direction(LEFT);
  background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, bg);
  breakable1 = new Drawable(0.0f, 0.0f, BREAKS, 1, TILE, tiles);
  breakable2 = new Drawable(0.0f, 0.0f, BREAKS, 1, TILE, tiles);
  breakable2->set_cur_frame(2);
  breakable3 = new Drawable(0.0f, 0.0f, BREAKS, 1, TILE, tiles);
  breakable3->set_cur_frame(3);
  moveable = new Drawable(0.0f, 0.0f, BLOCKS, 1, TILE, tiles);
  //moveable->set_cur_frame(MOVEABLE_BLOCK);
  plat1 = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  plat2 = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  plat2->set_cur_frame(2);
  plat3 = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  plat2->set_cur_frame(3);
  plat4 = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  plat2->set_cur_frame(4);
  plat5 = new Drawable(0.0f, 0.0f, PLATS, 1, TILE, tiles);
  plat2->set_cur_frame(5);
  ladder = new Drawable(0.0f, 0.0f, LADDS, 1, TILE, tiles);
  paused_background = new Drawable(0.0f, 0.0f, 1, 1, BACKGROUND, pause_bg);
  map_image = new Drawable(PAUSE_MAP_X, PAUSE_MAP_Y, 1, 1, VARIABLE, mi);
  pointer = new Drawable(650, 270, 1, 1, VARIABLE, pi);
  goal = new Drawable(0.0f, 0.0f, LADDS, 1, TILE, tiles);
  goal->set_cur_frame(2);
  black_hole = new Drawable(0.0f, 0.0f, BH, 1, TILE, tiles);
  toxic_sludge = new Drawable(0.0f, 0.0f, BH, 1, TILE, tiles);
  toxic_sludge->set_cur_frame(T_SLUDGE_FRAME);
  smog = new Drawable(0.0f, 0.0f, BH, 1, TILE, tiles);
  smog->set_cur_frame(T_SLUDGE_FRAME);
  smog->set_shadow(true);
  bouncer_cr = new Drawable(0.0f, 0.0f, BOUNCERS, 1, TILE, tiles);
  bouncer_cl = new Drawable(0.0f, 0.0f, BOUNCERS, 1, TILE, tiles);
  bouncer_cl->change_direction(LEFT);
  bouncer_o = new Drawable(0.0f, 0.0f, BOUNCERS, 1, TILE, tiles);
  bouncer_o->set_cur_frame(2);
  rubber = new Drawable(0.0f, 0.0f, BLOCKS, 1, TILE, tiles);
  rubber->set_cur_frame(RUBBER_FRAME);
  circuit = new Drawable(0.0f, 0.0f, BLOCKS, 1, TILE, tiles);
  circuit->set_cur_frame(CIRCUIT_FRAME);
  n_check = new Drawable(0.0f, 0.0f, CHECKS, 1, TILE, tiles);
  o_check = new Drawable(0.0f, 0.0f, CHECKS, 1, TILE, tiles);
  o_check->set_cur_frame(OLD_CHECK_FRAME);
  paused_names = new Drawable(0.0f, 0.0f, 1, 1, VARIABLE, p_names);
  
  // place all drawables into vector
  // this order MUST match the order
  // of the tile_type enum
  vector<Drawable*> v;
  v.clear();
  v.push_back(background);
  v.push_back(block1);
  v.push_back(block2);
  v.push_back(block3);
  v.push_back(block4);
  v.push_back(block5);
  v.push_back(left_block);
  v.push_back(right_block);
  v.push_back(left_corner_block);
  v.push_back(right_corner_block);
  v.push_back(br_bl);
  v.push_back(bl_bl);
  v.push_back(rubber);
  v.push_back(ladder);
  v.push_back(circuit);
  v.push_back(plat1);
  v.push_back(plat2);
  v.push_back(plat3);
  v.push_back(plat4);
  v.push_back(plat5);
  v.push_back(breakable1);
  v.push_back(breakable2);
  v.push_back(breakable3);
  v.push_back(goal);
  v.push_back(black_hole);
  v.push_back(toxic_sludge);
  v.push_back(smog);
  v.push_back(bouncer_cr);
  v.push_back(bouncer_cl);
  v.push_back(bouncer_o);
  v.push_back(n_check);
  v.push_back(o_check);
  
  textures.clear();
  textures.push_back(ahnold);
  textures.push_back(jumper);
  textures.push_back(engineer);
  textures.push_back(kurt);
  textures.push_back(paris);
  
  textures_tr.push_back(ahnold);
  textures_tr.push_back(jumper);
  textures_tr.push_back(nums);
  textures_tr.push_back(ps_ic);
  textures_tr.push_back(engineer);
  textures_tr.push_back(paris);
  textures_tr.push_back(kurt);
  
  m = new Map(v);
  temp_string.str(""); temp_string << RESOURCES << LEVEL << level << "/" << MAP1;
  m->load_map(temp_string.str().c_str(), moveables, specials, tiles, textures,
              p, sound_system, musics, m_channel, effects, a_channel, collects);
  s = new Game_State(p, m, moveables, specials, numbers, sound_system);
  paused = new Pause_State(sound_system, (Game_State *)s, paused_background,
                           map_image, pointer, ps_ic, paused_names);
  
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
  if (loading)
  {
    return;
  }
  switch(key)
  {
    //case 27: // escape key
    //  control_screen->clean();
    //  if (s == tutorial)
    //  {
    //    tutorial->clean();
    //    tutorial->state_clean();
    //    delete tutorial;
    //  }
    //  else if (s == main_s)
    //  {
    //    main_s->clean();
    //    main_s->state_clean();
    //    delete main_s;
    //  }
    //  else
    //  {
    //    system_clean();
    //  }
    //  exit(0);
    //  break;
    case '\n':
    case '\r':
      if (control)
      {
        control = false;
      }
      else if (s == paused)
      {
        if (((Pause_State *)s)->get_selected() == 0) // return
        {
          ((Pause_State *)s)->reset_selected();
          s = stack.at(stack.size() - 1);
          ((Game_State *)s)->unpause_volume();
          stack.pop_back();
        }
        else if (((Pause_State *)s)->get_selected() == 1) // controls
        {
          control = true;
        }
        else if (((Pause_State *)s)->get_selected() == 2) // restart
        {
          loading = true;
          ((Pause_State*)s)->pause_sounds();
          system_clean();
          stack.clear();
          glutPostRedisplay();
          glutTimerFunc(25, initLevel, level);
        }
        else if (((Pause_State *)s)->get_selected() == 3) // quit
        {
          ((Pause_State*)s)->pause_sounds();
          system_clean();
          stack.clear();
          loading = true;
          glutPostRedisplay();
          glutTimerFunc(25, initMain, 0);
        }
      }
      else if (s == tutorial)
      {
        loading = true;
        glutPostRedisplay();
        ((Tutorial_State*)s)->pause_sound();
        ((Tutorial_State*)s)->reset();
        ((Tutorial_State*)s)->clean();
        delete tutorial;
        glutTimerFunc(25, initMain, 0);
      }
      else if (s == main_s)
      {
        level = ((Main_Menu_State*)s)->get_selected();
        if (level == 11) // Tutorial
        {
          loading = true;
          glutPostRedisplay();
          ((Main_Menu_State*)s)->pause_sound();
          ((Main_Menu_State*)s)->clean();
          delete main_s;
          glutTimerFunc(25, initTutorial, 0);
        }
        else if (level == 12) // Quit
        {
          control_screen->clean();
          main_s->clean();
          delete main_s;
          exit(0);
        }
        else if (level > last_level)
        {
          fprintf(stderr, "Error level %u is not yet implemented\n", level);
        }
        else
        {
          loading = true;
          glutPostRedisplay();
          ((Main_Menu_State*)s)->pause_sound();
          ((Main_Menu_State*)s)->clean();
          delete main_s;
          glutTimerFunc(25, initLevel, level);
        }
      }
      else
      {
        ((Game_State *)s)->pause_volume();
        ((Game_State *)s)->reset_keys();
        stack.push_back(s);
        s = paused;
      }
      return;
  }
  s->key_pressed(key, x, y);
}

void handleKeyrelease(unsigned char key, int x, int y)
{
  if (loading)
  {
    return;
  }
  s->key_released(key, x, y);
}

void handleSpecialpress(int key, int x, int y)
{
  if (loading)
  {
    return;
  }
  s->special_pressed(key, x, y);
}

void handleSpecialrelease(int key, int x, int y)
{
  if (loading)
  {
    return;
  }
  s->special_released(key, x, y);
}

// called ever 25 milliseconds to update the screen
void update(int delta)
{
  if (!loading)
  {
    s->update(delta);
    if (s == tutorial)
    {
      if (delta < 0)
      {
        loading = true;
        ((Tutorial_State*)tutorial)->pause_sound();
        ((Tutorial_State*)tutorial)->reset();
        ((Tutorial_State*)tutorial)->clean();
        delete tutorial;
        glutTimerFunc(25, initMain, 0);
      }
    }
    else if (s != main_s)
    {
      // this means the goal state has been reached
      if (delta < 0)
      {
        loading = true;
        ((Game_State*)s)->pause_sounds();
        system_clean();
        if (++level <= last_level)
        {
          glutTimerFunc(25, initLevel, level);
        }
        else
        {
          printf("You win!\n");
          exit(0);
        }
        delta = 0;
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
  else if (control)
  {
    control_screen->draw();
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
  for (unsigned int i = 0; i < sounds.size(); ++i)
  {
    FMOD_RESULT result;
    
    result = FMOD_Sound_Release(sounds.at(i));
    ERRCHECK(result);
  }
  sounds.clear();
  
  for (unsigned int i = 0; i < textures_tr.size(); ++i)
  {
    delete textures_tr.at(i);
  }
  textures_tr.clear();
  
  paused->clean();
  delete paused;
}
