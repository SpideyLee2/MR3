#version 450 core

out vec4 FragColor;

uniform vec3 color;

void main() {
	float avg = (color.r + color.g + color.b) / 3.0;
	vec3 c = color / avg;
	FragColor = vec4(c, 1.0);
}