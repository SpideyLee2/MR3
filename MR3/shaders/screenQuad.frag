#version 450 core

in VS_OUT {
	vec2 texCoords;
} fs_in;

out vec4 FragColor;
uniform sampler2D quadTex;

void main() {
	FragColor = texture(quadTex, fs_in.texCoords);
}