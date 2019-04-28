#version 420

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 char_loc;

out vec2 char_uv;

void main() {
  gl_Position = vec4(vertex_position, 0.0, 1.0);
  char_uv = char_loc;
}