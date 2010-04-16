#ifdef __APPLE__
  #include <OpenGL/OpenGL.h>
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "drawable.h"
#include "defines.h"

Drawable::Drawable(void)
:tl_x(0), tl_y(0), width(0), height(0), tex_num(0), num_frames(0),
cur_frame(0), texture(0), facing_right(false)
{
  // initializes the corners to 0
  int i = 0, j = 0;
  for (;i < 2; ++i)
  {
    for(;j < 2; ++j)
    {
      corners[i][j] = 0.0f;
    }
  }
}

Drawable::Drawable(float x, float y, int num, int frames, D_TYPE type, Texture *tex)
:tl_x(x), tl_y(y), tex_num(num), num_frames(frames), cur_frame(1), texture(tex),
facing_right(true)
{
  switch (type)
  {
    // this is for drawing purposes
    // a tile only needs to take up a small space
    // and should be moved
    case TILE:
      width = TILE_WIDTH;
      height = TILE_HEIGHT;
      break;
    // background will never move and takes
    // up the entire screen
    case BACKGROUND:
      width = SCREEN_WIDTH;
      height = SCREEN_HEIGHT;
      break;
    case VARIABLE:
      width = texture->get_width();
      height = texture->get_height();
      break;
    case NUM:
      width = NUM_WIDTH;
      height = NUM_HEIGHT;
      break;
    case MAP_ICON:
      width = ICON_WIDTH;
      height = ICON_HEIGHT;
      break;
  }
  reset_corners();
}

// whenever the object's image is changed
// the corners of the points on the
// texture need to be recalculated
void Drawable::reset_corners(void)
{
  // if there is no texture (such as for a map)
  // there is no need to calculate corners since
  // nothing will ever be drawn
  if (texture == 0)
  {
    return;
  }
  float w = texture->get_width();
  float h = texture->get_height();
  corners[0][0] = (width * (cur_frame - 1)) / w;
  corners[0][1] = (width * (cur_frame)) / w;
  corners[1][1] = (height * (tex_num - 1)) / h;
  corners[1][0] = (height * (tex_num)) / h;
}

// draws the object to the screen
void Drawable::draw(void)
{
  // if there is no texture, nothing can be
  // drawn, so just retuen
  if (texture == 0)
  {
    return;
  }
  
  // otherwise it's time for some OpenGL magic
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texture->get_texture());
  glTranslatef(tl_x, tl_y, 0.0);
  
  // Begin drawing a rectangular object
  glBegin(GL_QUADS);
  
  if (facing_right)
  {
    // top left
    glTexCoord2d(corners[0][0], corners[1][1]);
    glVertex3i(0, 0, 0);
    
    // top right
    glTexCoord2d(corners[0][1], corners[1][1]);
    glVertex3i(width, 0, 0);
    
    // bottom right
    glTexCoord2d(corners[0][1], corners[1][0]);
    glVertex3i(width, height, 0);
    
    // bottom left
    glTexCoord2d(corners[0][0], corners[1][0]);
    glVertex3i(0, height, 0);
  }
  else
  {
    // top left
    glTexCoord2d(corners[0][1], corners[1][1]);
    glVertex3i(0, 0, 0);
    
    // top right
    glTexCoord2d(corners[0][0], corners[1][1]);
    glVertex3i(width, 0, 0);
    
    // bottom right
    glTexCoord2d(corners[0][0], corners[1][0]);
    glVertex3i(width, height, 0);
    
    // bottom left
    glTexCoord2d(corners[0][1], corners[1][0]);
    glVertex3i(0, height, 0);
  }
  
  glEnd();
  glPopMatrix();
}

// move the object by the offsets
void Drawable::move(float off_x, float off_y)
{
  tl_x += off_x;
  tl_y += off_y;
}

// put's the top left (x, y) of object into x and y
void Drawable::get_top_left(float &x, float &y)
{
  x = tl_x;
  y = tl_y;
}

// this is used to change the image of the object
void Drawable::set_tex_num(int tn)
{
  // if the texture number has changed
  // then the corners need to be recalculated
  if (tex_num != tn)
  {
    tex_num = tn;
    reset_corners();
  }
}

// changes the frame for animation images
void Drawable::set_cur_frame(int cf)
{
  if (cur_frame != cf)
  {
    cur_frame = cf;
    reset_corners();
  }
}

void Drawable::change_direction(direc dir)
{
  if (dir == LEFT)
  {
    facing_right = false;
  }
  else if (dir == RIGHT)
  {
    facing_right = true;
  }
}

void Drawable::clean(void)
{
  delete texture;
}
