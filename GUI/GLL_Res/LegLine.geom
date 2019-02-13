#version 420

layout (lines) in;
layout (triangle_strip, max_vertices = 10) out;

in float radius[];

uniform mat4 transform;
uniform vec3 cam_eye;

void main() {
	vec3 dirVec = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	vec3 ray = normalize(cross(dirVec, gl_in[0].gl_Position.xyz - cam_eye));
	vec3 ray2 = normalize(cross(dirVec, ray));

	gl_Position = transform * (gl_in[0].gl_Position +vec4(-ray-ray2-dirVec,0)*radius[0]);
	//elem_color = vec4(0,0,0,1);
	EmitVertex();

	gl_Position = transform * (gl_in[1].gl_Position +vec4(-ray-ray2+dirVec,0)*radius[1]);
	//elem_color = vec4(0,0,1,1);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position +vec4(-ray+ray2-dirVec,0)*radius[0]);
	//elem_color = vec4(0,1,0,1);
	EmitVertex();

	gl_Position = transform * (gl_in[1].gl_Position +vec4(-ray+ray2+dirVec,0)*radius[1]);
	//elem_color = vec4(0,1,1,1);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position +vec4(+ray+ray2-dirVec,0)*radius[0]);
	//elem_color = vec4(1,1,0,1);
	EmitVertex();

	gl_Position = transform * (gl_in[1].gl_Position +vec4(+ray+ray2+dirVec,0)*radius[1]);
	//elem_color = vec4(1,1,1,1);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position +vec4(+ray-ray2-dirVec,0)*radius[0]);
	//elem_color = vec4(1,0,0,1);
	EmitVertex();

	gl_Position = transform * (gl_in[1].gl_Position +vec4(+ray-ray2+dirVec,0)*radius[1]);
	//elem_color = vec4(1,0,1,1);
	EmitVertex();

	gl_Position = transform * (gl_in[0].gl_Position +vec4(-ray-ray2-dirVec,0)*radius[0]);
	//elem_color = vec4(0,0,0,1);
	EmitVertex();

	gl_Position = transform * (gl_in[1].gl_Position +vec4(-ray-ray2+dirVec,0)*radius[1]);
	//elem_color = vec4(0,0,1,1);
	EmitVertex();

	EndPrimitive();

}