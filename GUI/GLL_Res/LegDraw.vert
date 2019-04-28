#version 420

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;

uniform vec2 scale;
uniform vec2 offset;

out vec4 col;

void main() {
  col = vertex_color;
  gl_Position = vec4(((vertex_position - offset)/scale-1), 0.0, 1.0);
}