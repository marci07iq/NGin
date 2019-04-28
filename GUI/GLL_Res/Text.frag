#version 420

in vec2 char_uv;

uniform sampler2D font;
uniform vec4 color;

out vec4 frag_colour;

void main() {
  frag_colour = color;
  frag_colour.a = texture(font, char_uv).g;
  //frag_colour = mix(color, texture(font, char_uv), texture(font, char_uv).a);
}