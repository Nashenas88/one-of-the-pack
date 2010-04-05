#ifndef FLOCK__DEFINES__H
#define FLOCK__DEFINES__H

#define GAME_NAME "One of the Pack"

// speeds
#define PLAYER_SPEED 10
#define GRAVITY_SPEED 20

#define DIST_CHECK 30
#define FOLLOW_DIST 300
#define TOO_CLOSE 100

#define RESOURCES "resources/"
#define LEVEL "Level "

// texture files
#define TILE_TEXTURE "tiles.png"
#define BACKGROUND_TEXTURE "bg.png"
#define PAUSED_BACKGROUND "pause_background.png"
#define POINTER_TEXTURE "pointer.png"
#define MAP1 "level.bmp"
#define MAP_IMAGE "level.png"
#define LOADING_TEXTURE "loading.png"

// character texture files
#define PLAYER_TEXTURE "Character animation 1.png"
#define AHNOLD_TEXTURE "Ahnold.png"
#define JUMPER_TEXTURE "Funblob.png"

// sound file stuff
#define NUM_SOUNDS_FILE "sounds.txt"
#define SOUND_FILE_TYPE ".mp3"

// sound files
#define PLAYER_SOUND "Default.mp3"
#define AHNOLD_SFX "punch sound.wav"
#define JUMPER_SFX "jump sound (not final).wav"

// widths and heights
#define TILE_WIDTH 100
#define TILE_HEIGHT 100

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

#define MENU_ITEMS 3
#define POINTER_MOVE 90

#define DELTA_DELAY 4
#define SPECIAL_DELTA_DELAY 12

#define BORDER 100
#define COLLISION_SPACE 0.8f;

#define SOUND_CHANNEL 0
#define PAUSE_VOLUME 0.25f
#define UNFOCUSED_VOLUME 0.5f
#define MAX_VOLUME 1.0f

#define AHNOLD_PUNCH_NUM 5
#define PUNCH_RANGE 50

#define JUMPER_JUMP_NUM 5
#define JUMP_HEIGHT 100

#define LAST_LEVEL 3

typedef enum
{
  UP = 1,
  RIGHT,
  DOWN,
  LEFT
} direc;

typedef enum
{
  BG,
  BLOCK,
  L_BLOCK,
  R_BLOCK,
  LC_BLOCK,
  RC_BLOCK,
  LADDER,
  PLATFORM,
  BREAKABLE,
  GOAL
} tile_type;

enum
{
  RIGHT_BLOCK_FRAME = 2,
  LEFT_BLOCK_FRAME,
  LEFT_CORNER_BLOCK_FRAME,
  RIGHT_CORNER_BLOCK_FRAME
};

enum
{
  BLOCKS = 1,
  BREAKS,
  PLATS,
  LADDS
};

enum
{
  SPECIAL = 1,
  ABILITY
};

typedef enum
{
  AHNOLD,
  JUMPER
} special_type;

#endif // FLOCK__DEFINES__H
