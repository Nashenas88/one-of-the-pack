#include "tutorial_state.h"

Tutorial_State::Tutorial_State(void)
: State() {}

Tutorial_State::Tutorial_State(vector<Drawable *> images, FMOD_SYSTEM *s,
                                 FMOD_SOUND *mu, FMOD_CHANNEL *mch)
:State(s), slides(images), current_slide(0), music(mu), channel(mch)
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
  if (music == NULL || channel == NULL)
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

void Tutorial_State::clean(void)
{
  for (unsigned int i = 0; i < slides.size(); ++i)
  {
    slides.at(i)->clean();
    delete slides.at(i);
  }
  slides.clear();
  
  FMOD_RESULT result;
  
  result = FMOD_Sound_Release(music);
  ERRCHECK(result);
}
