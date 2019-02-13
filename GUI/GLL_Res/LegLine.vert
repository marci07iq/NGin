#version 420

layout(location = 0) in vec3 vertex_a;
layout(location = 1) in vec3 vertex_b;
layout(location = 4) in float line_radius;

out float radius;

void main() {
	radius = line_radius;
	gl_Position = vec4(vertex_a, 1);
}
