#version 420

in vec2 tex_uv;

uniform sampler2D tex;

out vec4 frag_colour;

void main() {
  frag_colour = texture(tex, tex_uv);
}