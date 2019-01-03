#version 420

layout(location = 0) in vec2 vertex_position;
uniform vec2 scale;

void main() {
  gl_Position = vec4((vertex_position/scale-1), 0.0, 1.0);
}