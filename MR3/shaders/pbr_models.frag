#version 450 core
// This shader is heavily inspired by Epic Games' Unreal Engine 4 PBR, and uses similar, if not the same, methodologies.
out vec4 FragColor;

in VS_OUT {
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

struct Material {
	sampler2D albedoMap;
	sampler2D normalMap;
	sampler2D metallicnessMap;
	sampler2D roughnessMap;
	sampler2D aoMap;
};

struct PointLight {
	vec3 position;
	vec3 color;
};

uniform Material material;
uniform PointLight pointLights[4];
uniform vec3 cameraPos;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

vec3 getNormalFromMap();
float distributionGGX(vec3 n, vec3 h, float r);
float smithGeometryGGX(vec3 n, vec3 v, vec3 l, float k);
float geometryGGX(vec3 n, vec3 v, float k);
vec3 fresnelApprox(vec3 h, vec3 v, vec3 f0);
vec3 fresnelApproxRoughness(vec3 h, vec3 v, vec3 f0, float roughness);
//vec3 fresnelApprox(vec3 h, vec3 n, vec3 f0);

const float PI = 3.14159265359;

void main() {
	vec3 albedo			= pow(texture(material.albedoMap, fs_in.texCoords).rgb, vec3(2.2));
	vec3 normal			= getNormalFromMap();
	float metallicness	= texture(material.metallicnessMap, fs_in.texCoords).r;
	float roughness		= texture(material.roughnessMap, fs_in.texCoords).r;
	float ao			= texture(material.aoMap, fs_in.texCoords).r;

	vec3 viewDir = normalize(cameraPos - fs_in.worldPos);
	vec3 f0	= mix(vec3(0.04), albedo, metallicness);
	vec3 reflectDir = reflect(-viewDir, normal);
	
	// Solves the integral over the hemisphere for direct light sources ONLY
	vec3 irradiance = vec3(0.0);
	for (int i = 0; i < 4; ++i) {
		// Per-light Radiance
		vec3 lightDir		= normalize(pointLights[i].position - fs_in.worldPos);
		vec3 halfwayDir		= normalize(viewDir + lightDir);
		float dist			= length(pointLights[i].position - fs_in.worldPos);
		float attenuation	= 1.0 / (dist * dist); // Inverse-square law
		vec3 radiance		= pointLights[i].color * attenuation;

		// Cook-Torrance BRDF
		float ndf		= distributionGGX(normal, halfwayDir, roughness);
		vec3 fresnel	= fresnelApprox(halfwayDir, viewDir, f0);
		float geometry	= smithGeometryGGX(normal, viewDir, lightDir, roughness);
		vec3 brdf		= (ndf * fresnel * geometry) / ((4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0)) + 0.0001);

		// Specular and Diffuse intensities
		vec3 kSpec = fresnel;
		vec3 kDiff = (vec3(1.0) - kSpec) * (1.0 - metallicness);

		// Add to Irradiance
		float NdotL = max(dot(normal, lightDir), 0.0);
		irradiance += (kDiff * albedo / PI + brdf) * radiance * NdotL;
	}
	// IBL ambient lighting
	vec3 kS = fresnelApproxRoughness(normal, viewDir, f0, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallicness;

	vec3 irrad = texture(irradianceMap, normal).rgb;
	vec3 diff = irrad * albedo;
	
	// TODO: This const should probably be a uniform since it depends on the number of mip levels of the prefilter map.
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, reflectDir, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
	
	vec3 ambient = (kD * diff + specular) * ao;

	vec3 color = ambient + irradiance;

	// HDR Tonemapping
	color = color / (color + vec3(1.0));
	// Gamma Correction
	color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, 1.0);
}

vec3 getNormalFromMap() {
	vec3 tangentNormal = texture(material.normalMap, fs_in.texCoords).xyz * 2.0 - 1.0;

	vec3 q1		= dFdx(fs_in.worldPos);
	vec3 q2		= dFdy(fs_in.worldPos);
	vec2 st1	= dFdx(fs_in.texCoords);
	vec2 st2	= dFdy(fs_in.texCoords);

	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 N = normalize(fs_in.normal);
	vec3 B = -normalize(cross(N, T));

	return normalize(mat3(T, B, N) * tangentNormal);
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

// Fresnel-Schlick approximation function 
// h = halfway direction, v = view direction, f0 = surface reflection at zero incidence;
vec3 fresnelApprox(vec3 h, vec3 v, vec3 f0) {
	return f0 + (1.0 - f0) * pow(clamp(1.0 - max(dot(h, v), 0.0), 0.0, 1.0), 5.0);
}
// Fresnel-Schlick approximation with injected roughness, as described by Sebastien Lagarde
vec3 fresnelApproxRoughness(vec3 h, vec3 v, vec3 f0, float roughness) {
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - max(dot(h, v), 0.0), 0.0, 1.0), 5.0);
}
//vec3 fresnelApprox(vec3 h, vec3 n, vec3 f0) {
//	// ??? learnopengl says this but the equation says the above
//	return f0 + (1.0 - f0) * pow(clamp(1.0 - max(dot(n, h), 0.0), 0.0, 1.0), 5.0); 
//}