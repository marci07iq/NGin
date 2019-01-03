#version 420

uniform vec4 color;

out vec4 frag_colour;

void main() {
  frag_colour = color;
}