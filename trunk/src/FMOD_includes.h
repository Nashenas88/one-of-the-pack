#ifndef FLOCK__FMOD_INCLUDES__H
#define FLOCK__FMOD_INCLUDES__H

#ifdef __APPLE__
  #include "/Developer/FMOD Programmers API Mac/api/inc/fmod.hpp"
  #include "/Developer/FMOD Programmers API Mac/api/inc/fmod_errors.h"
  #include "/Developer/FMOD Programmers API Mac/examples/common/wincompat.h"
  #include <stdio.h>
#else
  #include "C:/Program Files/FMOD SoundSystem/FMOD Programmers API Win32/api/inc/fmod.hpp"
  #include "C:/Program Files/FMOD SoundSystem/FMOD Programmers API Win32/api/inc/fmod_errors.h"
  #include <windows.h>
#endif

void ERRCHECK(FMOD_RESULT result);

#endif // FLOCK__FMOD_INCLUDES__H
