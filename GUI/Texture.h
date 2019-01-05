#pragma once

#include <GL/glew.h>
//#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include "../GL_Lib/GLFW/glfw3.h"

#include "ClientCore.h"
#include "../Maths/Point.h"
#include "stb_image.h"

#ifdef _WIN64
#pragma comment(lib, "glew64.lib")
#else
#ifdef WIN32
#pragma comment(lib, "glew32.lib")
#endif
#endif


GLuint png_texture_load(string filename, int& w, int& h);


class Texture {
public:
  GLuint id;
  iVec2 size;

  Texture() {

  }
  void load(string filename) {
    if (endsWith(filename, "png")) {
      id = png_texture_load(filename, size.x, size.y);
    } else {
      cout << "Unsupported file format" << endl;
    }
  }

  void bind(GLuint uniformId, int unit = 0) {
    //glBindTexture(GL_TEXTURE_2D, id);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
    glUniform1i(uniformId, unit);

  }

  ~Texture() {
    if(id != 0) {
      glDeleteTextures(1, &id);
    }
  }
};