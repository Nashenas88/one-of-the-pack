#ifndef FLOCK__DEFINES__H
#define FLOCK__DEFINES__H

#define GAME_NAME "One of the Pack"

// speeds
#define PLAYER_SPEED 10
#define GRAVITY_SPEED 20

#define DIST_CHECK 30

// texture files
#define TILE_TEXTURE "resources/tiles2.png"
#define BACKGROUND_TEXTURE "resources/bg.png"
#define PAUSED_BACKGROUND "resources/pause_background.png"
#define PLAYER_TEXTURE "resources/Flock you! Ulysses the Blob.png"
#define MAP1 "resources/blob sample level.bmp"

// sound files
#define PLAYER_SOUND "resources/Level 1 - Default.wav"

// widths and heights
#define TILE_WIDTH 100
#define TILE_HEIGHT 100

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

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

#endif // FLOCK__DEFINES__H
