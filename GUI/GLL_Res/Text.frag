#version 420

in vec2 char;

uniform sampler2D font;
uniform vec4 color;

out vec4 frag_colour;

void main() {
  frag_colour = color;
  frag_colour.a = texture(font, char).g;
}