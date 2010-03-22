#include <vector>
using namespace std;

#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

#include "texture.h"
#include "defines.h"

#include "SOIL.h"

Texture::Texture()
:width(0), height(0), texture(0) {}

Texture::Texture(const char *filename)
{
  loadTexture(filename);
}

// loads the texture from any image file
void Texture::loadTexture (const char *filename)
{
  int channels;
  unsigned char* data;
  data = SOIL_load_image(filename, &width, &height, &channels, 4);
  
  // allocate a texture name
  glGenTextures( 1, &texture );
  
  // bind texture and apply parameters
  glBindTexture (GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  //glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)&data[0]);
  // build our texture mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA,
                    GL_UNSIGNED_BYTE, (unsigned char *)&data[0]);
  free(data);
}
