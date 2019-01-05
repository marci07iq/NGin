#include "Texture.h"


GLuint png_texture_load(string filename, int& w, int& h) {
  int comp;
  unsigned char* image = stbi_load(trimStr(filename).c_str(), &w, &h, &comp, STBI_rgb_alpha);

  if (image == nullptr)
    throw(std::string("Failed to load texture"));

  GLuint m_texture = 0;

  glGenTextures(1, &m_texture);

  glBindTexture(GL_TEXTURE_2D, m_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  if (comp == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  else if (comp == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  //glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);

  return m_texture;
}
