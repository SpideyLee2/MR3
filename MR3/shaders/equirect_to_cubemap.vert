#version 450 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
	mat4 view;
	mat4 projection;
};

out VS_OUT {
	vec3 localPos;
} vs_out;

void main() {
	vs_out.localPos = aPos;
	gl_Position = projection * view * vec4(aPos, 1.0);
}