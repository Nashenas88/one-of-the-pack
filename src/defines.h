#ifndef FLOCK__DEFINES__H
#define FLOCK__DEFINES__H

#define GAME_NAME "One of the Pack"

// speeds
#define PLAYER_SPEED 10
#define BEAM_SPEED 20
#define GRAVITY_SPEED 20

#define DIST_CHECK 30
#define FOLLOW_DIST 300
#define TOO_CLOSE 100

#define RESOURCES "resources/"
#define LEVEL "Level "

// texture files
#define NUMBER_TEXTURE "numbers.png"
#define TILE_TEXTURE "tiles.png"
#define BACKGROUND_TEXTURE "bg.png"
#define PAUSED_BACKGROUND "pause_background.png"
#define POINTER_TEXTURE "pointer.png"
#define MAP1 "level.bmp"
#define MAP_IMAGE "level.png"
#define LOADING_TEXTURE "loading.png"
#define PAUSE_ICONS_TEXTURE "pause_icons.png"
#define PAUSED_NAMES_TEXTURE "names.png"

// character texture files
#define PLAYER_TEXTURE "Character animation 1.png"
#define AHNOLD_TEXTURE "Ahnold.png"
#define JUMPER_TEXTURE "Funblob.png"
#define ENGINEER_TEXTURE "Thinkblob.png"
#define BEAM_ANIM 3
#define PARIS_TEXTURE "Cutieblob.png"
#define KURT_TEXTURE "Wonderblob.png"

// sound file stuff
#define NUM_SOUNDS_FILE "sounds.txt"
#define SOUND_FILE_TYPE ".mp3"

// sound files
#define PLAYER_SOUND "Default.mp3"
#define AHNOLD_SFX "punch sound.wav"
#define JUMPER_SFX "jump sound (not final).wav"
#define ENGINEER_SFX "jump sound (not final).wav"
#define BEAM_SFX "punch sound.wav"
#define KURT_SFX "jump sound (not final).wav"

// widths and heights
#define TILE_WIDTH 100
#define TILE_HEIGHT 100

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800

#define NUM_WIDTH 50
#define NUM_HEIGHT 50

#define ICON_WIDTH 8
#define ICON_HEIGHT 8

#define MENU_ITEMS 3
#define POINTER_MOVE 90

#define DELTA_DELAY 2
#define SPECIAL_DELTA_DELAY 3
#define AHNOLD_DELTA_DELAY 0
#define ENGINEER_DELTA_DELAY 0
#define PARIS_DELTA_DELAY 1
#define BEAM_DELTA_DELAY 2

#define BORDER 100
#define COLLISION_SPACE 0.8f;

#define SOUND_CHANNEL 0
#define PAUSE_VOLUME 0.25f
#define UNFOCUSED_VOLUME 0.5f
#define MAX_VOLUME 1.0f

#define AHNOLD_PUNCH_NUM 5
#define PUNCH_RANGE 30
#define BLOCK_SLIDE_SPEED 20
#define HOVER_SLIDE_SPEED 10

#define SLIDE_COUNTER 20
#define SMALLEST_MOVEMENT 1

#define JUMPER_JUMP_NUM 5
#define JUMP_HEIGHT 20

#define BEAM_NUM 3
#define ENGINEER_BUTTON_NUM 5

#define KURT_GUITAR_NUM 5
#define KURT_ABILITY_FRAMES 3
#define KURT_MUSIC_NUM 5

#define PARIS_KISS_NUM 4
#define PARIS_KISS_RANGE 15

#define LAST_LEVEL 3

typedef enum
{
  UP = 1,
  RIGHT,
  DOWN,
  LEFT
} direc;

#define RUBBER_FRAME 2
#define CIRCUIT_FRAME 3
#define OLD_CHECK_FRAME 2
#define T_SLUDGE_FRAME 2
#define FREEZEABLE_FRAME 2
typedef enum
{
  BG,
  BLOCK1,
  BLOCK2,
  BLOCK3,
  BLOCK4,
  BLOCK5,
  R_BLOCK,
  L_BLOCK,
  LC_BLOCK,
  RC_BLOCK,
  RUBBER,
  LADDER,
  CIRCUIT,
  PLATFORM,
  BREAKABLE,
  GOAL,
  BLACK_HOLE,
  TOXIC_SLUDGE,
  BOUNCER_CLOSED_R,
  BOUNCER_CLOSED_L,
  BOUNCER_OPEN,
  NEW_CHECKPOINT,
  OLD_CHECKPOINT,
  OUTSIDE
} tile_type;

enum
{
  RIGHT_WALL_FRAME = 1,
  LEFT_WALL_FRAME,
  LEFT_CORNER_WALL_FRAME,
  RIGHT_CORNER_WALL_FRAME
};

enum
{
  FLOORS = 1,
  SIDES_CORNERS,
  BLOCKS,
  PLATS,
  LADDS,
  GOALT,
  PUSH,
  BH,
  BOUNCERS,
  CHECKS
};

enum
{
  SPECIAL = 1,
  ABILITY
};

typedef enum
{
  AHNOLD,
  JUMPER,
  ENGINEER,
  KURT,
  PARIS,
  BEAM
} special_type;

#define PAUSE_ICON_FRAMES 4
typedef enum
{
  PLAYER_ICON = 1,
  AHNOLD_ICON,
  JUMPER_ICON,
  KURT_ICON,
  PARIS_ICON,
  ENGINEER_ICON,
  BREAK_ICON,
  PUSH_ICON,
  GOAL_ICON,
  HOVER_ICON
} pause_icon;

#define PAUSE_MAP_X 300
#define PAUSE_MAP_Y 300
#define PAUSE_ANIM_DELAY 4

#define JUMP_DELAY 6

#endif // FLOCK__DEFINES__H
