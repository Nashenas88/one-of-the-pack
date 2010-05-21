#include "state.h"

State::State(void)
: system(0) {}

State::State(FMOD_SYSTEM *sys)
: system(sys) {}

State::~State(void) {}

void State::draw(void) {}

void State::update(int &delta) {}

void State::state_update(void)
{
  FMOD_System_Update(system);
}

void State::key_pressed(unsigned char key, int x, int y) {}

void State::key_released(unsigned char key, int x, int y) {}

void State::special_pressed(int key, int x, int y) {}

void State::special_released(int key, int x, int y) {}
