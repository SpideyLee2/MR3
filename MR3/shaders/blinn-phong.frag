#version 450 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};
uniform Material material;

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLight;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

uniform vec3 viewPos;

in VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

out vec4 FragColor;

vec3 calcDirLight(DirLight light, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 viewDir);

void main() {
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);

	vec3 color = vec3(0.0);

	color += calcDirLight(dirLight, viewDir);
	color += calcPointLight(pointLight, viewDir);

	FragColor = vec4(color, 1.0);

	// Gamma correction
	//float gamma = 2.2;
	//FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
}

vec3 calcDirLight(DirLight light, vec3 viewDir) {
	vec3 lightDir = normalize(light.direction);

	// Ambient Lighting
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texCoords));

	// Diffuse Lighting
	float diff = max(dot(lightDir, fs_in.normal), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texCoords));

	// Blinn-Phong Specular Lighting
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));
	// Blinn-Phong Specular Lighting
	//const float ENERGY_CONSERVATION = ( 8.0 + material.shininess ) / ( 8.0 * 3.14159265 ); 
	//vec3 halfwayDir = normalize(lightDir + viewDir); 
	//float spec = ENERGY_CONSERVATION * pow(max(dot(fs_in.normal, halfwayDir), 0.0), material.shininess);
	//vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));

	return (ambient + diffuse + specular) * vec3(texture(material.diffuse, fs_in.texCoords));
}

vec3 calcPointLight(PointLight light, vec3 viewDir) {
	// Attenuation
    float distance = length(light.position - fs_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Ambient Lighting
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texCoords));

	// Diffuse Lighting
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	float diff = max(dot(fs_in.normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texCoords));

	// Blinn-Phong Specular Lighting
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(dot(fs_in.normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));
	// Blinn-Phong Specular Lighting
	//const float ENERGY_CONSERVATION = ( 8.0 + material.shininess ) / ( 8.0 * 3.14159265 ); 
	//vec3 halfwayDir = normalize(lightDir + viewDir); 
	//float spec = ENERGY_CONSERVATION * pow(max(dot(fs_in.normal, halfwayDir), 0.0), material.shininess);
	//vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texCoords));

    return (ambient + diffuse + specular) * vec3(texture(material.diffuse, fs_in.texCoords)) * attenuation;
}