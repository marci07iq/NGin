#include "Shader.h"

Shader::Shader() {

}

void Shader::create(string files, int flags) {
  if (flags & 1) _v_shaderID = compileShader(files + ".vert", GL_VERTEX_SHADER);
  if (flags & 2) _g_shaderID = compileShader(files + ".geom", GL_GEOMETRY_SHADER);
  if (flags & 4) _f_shaderID = compileShader(files + ".frag", GL_FRAGMENT_SHADER);

  _pID = glCreateProgram();
  if (flags & 4) glAttachShader(_pID, _f_shaderID);
  if (flags & 2) glAttachShader(_pID, _g_shaderID);
  if (flags & 1) glAttachShader(_pID, _v_shaderID);

  glLinkProgram(_pID);
  glValidateProgram(_pID);

  checkError(_pID, GL_LINK_STATUS, true, "Shader link failed");

  cout << "Created shader program " << _pID << endl;
}

void Shader::bind() {
  glUseProgram(_pID);
}

void Shader::unbind() {
  glUseProgram(0);
}

Shader::~Shader() {
  cout << "Delete shader program " << _pID << endl;
  if(_pID != 0) {
    if (_v_shaderID) glDetachShader(_pID, _v_shaderID);
    if (_g_shaderID) glDetachShader(_pID, _g_shaderID);
    if (_f_shaderID) glDetachShader(_pID, _f_shaderID);

    _v_shaderID = 0;
    _g_shaderID = 0;
    _f_shaderID = 0;

    glDeleteProgram(_pID);
  }
}
