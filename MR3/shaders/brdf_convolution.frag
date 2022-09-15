#version 450 core

out vec2 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

vec2 integrateBRDF(float NdotV, float roughness);
vec2 hammersleySequence(uint i, uint n);
float radicalInverseVDC(uint bits);
vec3 importanceSampleGGX(vec2 lds, vec3 n, float roughness);
float smithGeometryGGX(vec3 n, vec3 v, vec3 l, float k);
float geometryGGX(vec3 n, vec3 v, float k);

const float PI = 3.14159265359;

void main() {
    FragColor = integrateBRDF(fs_in.texCoords.x, fs_in.texCoords.y);
}

vec2 integrateBRDF(float NdotV, float roughness) {
    vec3 viewDir;
    viewDir.x = sqrt(1.0 - NdotV * NdotV);
    viewDir.y = 0.0;
    viewDir.z = NdotV;

    float a = 0.0;
    float b = 0.0;

    vec3 normal = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // Generates a random sample vector that's biased towards the preferred alignment direction
        vec2 lds = hammersleySequence(i, SAMPLE_COUNT);
        vec3 halfwayDir = importanceSampleGGX(lds, normal, roughness);
        vec3 l = normalize(2.0 * dot(viewDir, halfwayDir) * halfwayDir - viewDir);

        float NdotL = max(l.z, 0.0);
        float NdotH = max(halfwayDir.z, 0.0);
        float VdotH = max(dot(viewDir, halfwayDir), 0.0);

        if (NdotL > 0.0) {
            float remappedRoughness = roughness * roughness / 2.0;
            float geometry = smithGeometryGGX(normal, viewDir, l, remappedRoughness);
            float geometryVis = (geometry * VdotH) / (NdotH * NdotV);
            float fc = pow(1.0 - VdotH, 5.0);

            a += (1.0 - fc) * geometryVis;
            b += fc * geometryVis;
        }
    }

    a /= float(SAMPLE_COUNT);
    b /= float(SAMPLE_COUNT);

    return vec2(a, b);
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

// Smith's Schlick-GGX geometry function
// n = surface normal, v = view direction, l = light direction, k = remapped roughness (direct/IBL)
float smithGeometryGGX(vec3 n, vec3 v, vec3 l, float k) {
	return geometryGGX(n, v, k) * geometryGGX(n, l, k);
}

// Schlick-GGX geometry function
// n = surface normal, v = view or light direction, k = remapped roughness (direct/IBL)
float geometryGGX(vec3 n, vec3 v, float k) {
	float NdotV = max(dot(n, v), 0.0);

	return NdotV / (NdotV * (1.0 - k) + k);
}