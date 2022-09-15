#version 450 core

out vec4 FragColor;

in VS_OUT {
	vec3 localPos;
} fs_in;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 pos);

void main() {
	// Converts position from spherical to cartesian to use as UV coords
	vec2 uv = sampleSphericalMap(normalize(fs_in.localPos));
	// Samples equirectangular map using new cartesian uv coords
	vec3 color = texture(equirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}

vec2 sampleSphericalMap(vec3 pos) {
	return vec2(atan(pos.z, pos.x), asin(pos.y)) * invAtan + 0.5;
}