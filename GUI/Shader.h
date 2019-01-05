#pragma once

#include "Texture.h"


class Shader {
public:
  GLuint _v_shaderID;
  GLuint _g_shaderID;
  GLuint _f_shaderID;
  GLuint _pID;

  static void checkError(GLuint shader, GLuint type, bool isProgram, string message) {
    GLint success = 0;
    GLchar error[1024] = {0};

    if (isProgram) {
      glGetProgramiv(shader, type, &success);
    } else {
      glGetShaderiv(shader, type, &success);
    }

    if (success == GL_FALSE) {
      if (isProgram) {
        glGetProgramInfoLog(shader, sizeof(error), NULL, error);
      } else {
        glGetShaderInfoLog(shader, sizeof(error), NULL, error);
      }
      cerr << message << ": " << error << endl;
    }
  }

  Shader();
  void create(string files, int flags = 5);

  static GLuint compileShader(string filename, GLenum shaderType) {
    ifstream vertex_file(filename);
    stringstream filess;
    filess << vertex_file.rdbuf();
    string files = filess.str();

    GLuint shader = glCreateShader(shaderType);
    
    if (shader == 0) {
      cerr << "ERROR: Shader creation failed." << endl;
    }
    const GLchar* shader_c[1];
    GLint shader_c_length[1];

    shader_c[0] = files.c_str();
    shader_c_length[0] = files.size();

    glShaderSource(shader, 1, shader_c, shader_c_length);
    glCompileShader(shader);

    checkError(shader, GL_COMPILE_STATUS, false, "Shader compile failed");

    return shader;
  }

  void bind();
  void unbind();

  ~Shader();
};