CC = g++

NAME = one_of_the_pack

SRC_DIR = src

ifeq ($(OS),Windows_NT)
	PROG = $(NAME).exe
	CLFAGS = -Wall -mno-cygwin
else
	PROG = $(NAME)
	CFLAGS = -Wall
endif

SRCS = drawable.cpp texture.cpp player.cpp moveable.cpp character.cpp \
	state.cpp map.cpp tutorial_state.cpp main_menu_state.cpp game_state.cpp \
  pause_state.cpp FMOD_includes.cpp main.cpp ahnold.cpp special.cpp \
  jumper.cpp engineer.cpp paris.cpp beam.cpp wonder.cpp
HDRS = defines.h

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT -framework CoreFoundation \
		-lmacSOIL
  FMOD_LIB_P = /Developer/FMOD\ Programmers\ API\ Mac/api/lib
  FMOD_LIB = libfmodex.dylib
  ARCH = -arch i386
else
	LIBS = -lglut -lSOIL
endif

ifeq ($(OS),Windows_NT)
	INCL = -I/usr/include/opengl -I/usr/include/w32api \
		-I./Simple\ OpenGL\ Image\ Library/src
	LIB_P = -L/usr/lib/w32api -L./Simple\ OpenGL\ Image\ Library/lib
	LIBS =  -lwinSOIL -lglut32 -lglu32 -lopengl32
  FMOD_LIB_P = /cygdrive/C/Program\ Files/FMOD\ SoundSystem/FMOD\ Programmers\ API\ Win32/api/lib
  FMOD_LIB = libfmodex.a
else
	INCL = -I./Simple\ OpenGL\ Image\ Library/src
	LIB_P = -L./Simple\ OpenGL\ Image\ Library/lib
endif

all: $(PROG)

$(PROG):	$(SRC_DIR)/*.cpp $(SRC_DIR)/*.h
	cd $(SRC_DIR); $(CC) $(ARCH) $(CFLAGS) -o $(PROG) $(INCL) $(SRCS) $(FMOD_LIB_P)/$(FMOD_LIB) $(LIB_P) $(LIBS); mv $(PROG) ../

clean:
	rm -f $(PROG)
