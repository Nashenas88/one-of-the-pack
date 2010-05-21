CC = g++

NAME = one_of_the_pack

ifeq ($(OS),Windows_NT)
	PROG = $(NAME).exe
	CLFAGS = -Wall -c -mno-cygwin
else
	PROG = $(NAME)
	CFLAGS = -Wall -c -g
endif

H_SRCS = drawable.cpp texture.cpp player.cpp moveable.cpp character.cpp \
	state.cpp map.cpp tutorial_state.cpp main_menu_state.cpp game_state.cpp \
	pause_state.cpp FMOD_includes.cpp ahnold.cpp special.cpp jumper.cpp \
	engineer.cpp paris.cpp beam.cpp wonder.cpp
SRCS = $(H_SRCS) main.cpp
OBJECTS = $(SRCS:.cpp=.o)
HDRS = $(H_SRCS:.cpp=.h) defines.h

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT -framework CoreFoundation \
		-lmacSOIL
	FMOD_LIB_P = /Developer/FMOD\ Programmers\ API\ Mac/api/lib
	FMOD_LIB = libfmodex.dylib
	ARCH = -arch i386
	APP_NAME = One\ of\ the\ Pack
	MAKE_APP = rm -rf ../$(APP_NAME).app; \
		mkdir ../$(APP_NAME).app; \
		mkdir ../$(APP_NAME).app/Contents; \
		mkdir ../$(APP_NAME).app/Contents/MacOS; \
		mkdir ../$(APP_NAME).app/Contents/Resources; \
		cp -R ../Mac\ OS\ X/*Info* ../$(APP_NAME).app/Contents/; \
		cp -R ../Mac\ OS\ X/*icns ../$(APP_NAME).app/Contents/Resources; \
		cp -R ../$(FMOD_LIB) ../$(APP_NAME).app/Contents/Resources/; \
		cp -R ../resources/* ../$(APP_NAME).app/Contents/Resources/; \
		cp ../$(PROG) ../$(APP_NAME).app/Contents/MacOS; \
		install_name_tool -change ./$(FMOD_LIB) @executable_path/../Resources/$(FMOD_LIB) ../$(APP_NAME).app/Contents/MacOS/$(PROG);
else
	LIBS = -lSOIL -lglut -lGLU -pthread
	FMOD_LIB_P = /usr/local/lib
	FMOD_LIB = libfmodex-4.30.03.so
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

ifeq ($(shell uname),Linux)
	INCL = -I/usr/local/include/fmodex
endif

all: ../$(PROG)

.cpp.o:
	$(CC) $(ARCH) -c $(CFLAGS) -o $@ $(INCL) $<;

../$(PROG): $(OBJECTS)
	$(CC) $(ARCH) -o ../$(PROG) $(OBJECTS) $(FMOD_LIB_P)/$(FMOD_LIB) $(LIB_P) $(LIBS)

ifeq ($(shell uname),Darwin)
app: ../$(PROG)
	rm -rf ../$(APP_NAME).app
	mkdir ../$(APP_NAME).app
	mkdir ../$(APP_NAME).app/Contents
	mkdir ../$(APP_NAME).app/Contents/MacOS
	mkdir ../$(APP_NAME).app/Contents/Resources
	cp -R ../Mac\ OS\ X/*Info* ../$(APP_NAME).app/Contents/
	cp -R ../Mac\ OS\ X/*icns ../$(APP_NAME).app/Contents/Resources
	cp -R ../$(FMOD_LIB) ../$(APP_NAME).app/Contents/Resources/
	cp -R ../resources/* ../$(APP_NAME).app/Contents/Resources/
	cp ../$(PROG) ../$(APP_NAME).app/Contents/MacOS
	install_name_tool -change ./$(FMOD_LIB) @executable_path/../Resources/$(FMOD_LIB) ../$(APP_NAME).app/Contents/MacOS/$(PROG)
endif

clean:
	rm -f $(OBJECTS)
