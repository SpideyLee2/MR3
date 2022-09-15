#version 450 core

out vec4 FragColor;

in VS_OUT {
	vec3 localPos;
} fs_in;

uniform samplerCube envMap;

void main() {
	vec3 envColor = texture(envMap, fs_in.localPos).rgb;

	// Environment map's HDR values were put into LDR framebuffer, so they need to be tone mapped
	// HDR tone mapping
	envColor = envColor / (envColor + vec3(1.0));
	// Gamma correction
	envColor = pow(envColor, vec3(1.0 / 2.2));

	FragColor = vec4(envColor, 1.0);
}