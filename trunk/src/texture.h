#ifndef FLOCK__TEXTURE__H
#define FLOCK__TEXTURE__H

#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

class Texture
{
private:
  int width; // width and height of texture
  int height;
  GLuint texture;
public:
  Texture();
  Texture(const char *filename);
  
  // loads texture from a file, and automagically sets up
  // width, height, and all OpenGL information
  void loadTexture (const char *filename);
  
  // getters
  int get_width(void) {return width;}
  int get_height(void) {return height;}
  GLuint get_texture(void) {return texture;}
};

#endif // FLOCK__TEXTURE__H
