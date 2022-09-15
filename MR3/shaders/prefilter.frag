#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 localPos;
} fs_in;

uniform samplerCube envMap;
uniform float roughness;

vec2 hammersleySequence(uint i, uint n);
float radicalInverseVDC(uint bits);
vec3 importanceSampleGGX(vec2 lds, vec3 n, float roughness);
float distributionGGX(vec3 n, vec3 h, float r);

const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(fs_in.localPos);
    // Simplifying assumption that viewDir = r = normal as proposed by Epic Games. Causes loss of grazing reflections,
    // but makes generating the prefilter map feasible.
    vec3 r = normal;
    vec3 viewDir = r;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // Generates a random sample vector that's biased towards the preferred alignment direction
        vec2 lds = hammersleySequence(i, SAMPLE_COUNT);
        vec3 halfwayDir = importanceSampleGGX(lds, normal, roughness);
        vec3 l = normalize(2.0 * dot(viewDir, halfwayDir) * halfwayDir - viewDir);

        // Adds to the prefiltered color the sampled environment map texel, scaled by its weight.
        // If the weight is 0, the sample vector is not within the specular lobe, and does not contribute to the final
        // prefiltered color.
        float weight = max(dot(normal, l), 0.0);
        if (weight > 0.0) {
            // Samples for a specific mip level dependant on the roughness to prevent color splotches, as proposed by
            // Chetan Jags
            float distrib = distributionGGX(normal, halfwayDir, roughness);
            float pdf = distrib * max(dot(normal, halfwayDir), 0.0) / (4.0 * max(dot(halfwayDir, viewDir), 0.0)) + 0.0001;
            float resolution = 512.0; // Resolution of source cubemap per face
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += texture(envMap, l).rgb * weight;
            totalWeight += weight;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}

// Returns the low-discrepancy sample i of the total sample set of size n.
vec2 hammersleySequence(uint i, uint n) {
	return vec2(float(i) / float(n), radicalInverseVDC(i));
}

// Returns the Van Der Corput sequence
float radicalInverseVDC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // bits / 0x100000000
}

// Returns a sample vector oriented around the microsurface's halfway vector based on the low-discrepancy sequence and 
// roughness.
vec3 importanceSampleGGX(vec2 lds, vec3 n, float roughness) {
    float r = roughness * roughness; // Squared for better visual results

    float phi = 2.0 * PI * lds.x;
    float cosTheta = sqrt((1.0 - lds.y) / (1.0 + (r * r - 1.0) * lds.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Spherical to Cartesian coords
    vec3 h;
    h.x = cos(phi) * sinTheta;
    h.y = sin(phi) * sinTheta;
    h.z = cosTheta;

    // Tangent space to World space
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitangent = cross(n, tangent);

    return normalize(tangent * h.x + bitangent * h.y + n * h.z);
}

// Trowbridge-Reitz GGX normal distribution function
// n = surface normal, h = halfway direction, r = roughness
float distributionGGX(vec3 n, vec3 h, float r) {
	float a = r * r;
	float a2 = a * a;
	float NdotH = max(dot(n, h), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = NdotH2 * (a2 - 1.0) + 1.0;

	return a2 / (PI * denom * denom);
}