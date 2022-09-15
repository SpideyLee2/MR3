#version 450 core

out vec4 FragColor;

in VS_OUT {
	vec3 localPos;
} fs_in;

uniform samplerCube envMap;

const float PI = 3.14159265359;

vec3 convolute(vec3 normal);

void main() {
	vec3 normal = normalize(fs_in.localPos);

	vec3 irradiance = convolute(normal);

	FragColor = vec4(irradiance, 1.0);
}

vec3 convolute(vec3 normal) {
	vec3 irradiance = vec3(0.0);

	// Calculates the up and right direction vectors
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	// Determines how many samples to take around the hemisphere. Lower = more samples.
	float sampleDelta = 0.025;
	float numSamples = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
			// Spherical to Cartesian coords (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// Tangent to World space, oriented around the normal
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
			// Sample HDR environment map
			// Scaled by cos(theta) to account for weak lighting at large angles
			// Scaled by sin(theta) to account for small sample areas at higher hemisphere areas
			// NOTE: I believe you can remove both the cos and sin scaling if using a icosahedron, but they
			// must be used in the case of a UV sphere.
			irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
			++numSamples;
		}
	}

	// Average sampled irradiance
	irradiance = PI * irradiance / numSamples;

	return irradiance;
}