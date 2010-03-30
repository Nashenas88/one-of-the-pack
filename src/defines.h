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
#define LEVEL1 "Level 1/"
#define LEVEL2 "Level 2/"
#define LEVEL3 "Level 3/"

// texture files
#define TILE_TEXTURE "tiles.png"
#define BACKGROUND_TEXTURE "bg.png"
#define PAUSED_BACKGROUND "pause_background.png"
#define POINTER_TEXTURE "pointer.png"
#define MAP1 "level.bmp"

// character texture files
#define PLAYER_TEXTURE "Character animation 1.png"
#define AHNOLD_TEXTURE "Ahnold.png"

#define NUM_SOUNDS_FILE "sounds.txt"
#define SOUND_FILE_TYPE ".mp3"

// sound files
#define PLAYER_SOUND "Default.mp3"
#define AHNOLD_SFX "punch sound.wav"

// widths and heights
#define TILE_WIDTH 100
#define TILE_HEIGHT 100

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

#define MENU_ITEMS 3
#define POINTER_MOVE 90

#define BORDER 100

#define SOUND_CHANNEL 0

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
  LADDER,
  PLATFORM,
  BREAKABLE,
  GOAL
} tile_type;

typedef enum
{
  AHNOLD
} specials;

#endif // FLOCK__DEFINES__H
