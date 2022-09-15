#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
	vec2 texCoords;
} vs_out;

void main() {
	vs_out.texCoords = aTexCoords;
	gl_Position = vec4(aPos, 0.0, 1.0);
}