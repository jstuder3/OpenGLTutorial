#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 Position;
	vec3 Color;
	float Constant;
	float Linear;
	float Quadratic;
	float MaxDistance;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main() {
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	vec3 lighting = Albedo * 0.1f; // ambient term
	vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; i++){
		float distance = length(lights[i].Position - FragPos);
		if(distance > lights[i].MaxDistance){
			continue;
		}
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		float attenuation = 1.0f / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0f) * Albedo * lights[i].Color;
		lighting += diffuse * attenuation;

		// Specular

		vec3 halfway = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfway), 0.0f), 32.0f);
		lighting += lights[i].Color * spec * Specular * attenuation;

	}

	FragColor = vec4(lighting, 1.0f);
	
}