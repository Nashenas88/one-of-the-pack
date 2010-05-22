#include "tutorial_state.h"

#include <iostream>
using namespace std;

Tutorial_State::Tutorial_State(void)
: State() {}

Tutorial_State::Tutorial_State(vector<Drawable *> images, FMOD_SYSTEM *s,
                                 FMOD_SOUND *mu, vector<Texture *> texs)
:State(s), slides(images), current_slide(0), music(mu), channel(NULL),
textures(texs)
{}

void Tutorial_State::draw(void)
{
  if (current_slide < slides.size())
  {
    slides.at(current_slide)->draw();
  }
  else
  {
    slides.at(slides.size() - 1)->draw();
  }
}

void Tutorial_State::update(int &delta)
{
  if (current_slide == slides.size())
  {
    delta = -1;
  }
  else
  {
    delta = 0;
  }
  state_update();
}

void Tutorial_State::key_pressed(unsigned char key, int x, int y)
{
  switch(key)
  {
    case ' ':
      ++current_slide;
      if (current_slide == 1)
      {
        play_sound();
      }
      break;
    default:
      break;
  }
}

void Tutorial_State::key_released(unsigned char key, int x, int y) {}

void Tutorial_State::special_pressed(int key, int x, int y) {}

void Tutorial_State::special_released(int key, int x, int y) {}

// plays the characters single sound
// (might possibly be renamed)
void Tutorial_State::play_sound(void)
{
  if (music == NULL)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  result = FMOD_System_PlaySound(get_system(), FMOD_CHANNEL_FREE, music,
                                 0, &channel);
  ERRCHECK(result);
}

// pauses (and unpauses) the characters single sound
// (might possibly be renamed)
void Tutorial_State::pause_sound(void)
{
  if (music == NULL || channel == NULL || current_slide == 0)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  result = FMOD_Channel_GetPaused(channel, &sound_paused);
  ERRCHECK(result);
  
  sound_paused = !sound_paused;
  
  result = FMOD_Channel_SetPaused(channel, sound_paused);
  ERRCHECK(result);
}

void Tutorial_State::pause_sound(bool b)
{
  if (music == NULL || channel == NULL || current_slide == 0)
  {
    return;
  }
  
  FMOD_RESULT result;
  
  sound_paused = b;
  
  result = FMOD_Channel_SetPaused(channel, sound_paused);
  ERRCHECK(result);
}

Tutorial_State::~Tutorial_State(void)
{
  for (unsigned int i = 0; i < slides.size(); ++i)
  {
    delete slides.at(i);
    slides.at(i) = 0;
  }
  slides.clear();
  
  for (unsigned int i = 0; i < textures.size(); ++i)
  {
    delete textures.at(i);
    textures.at(i) = 0;
  }
  textures.clear();
  
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
}
