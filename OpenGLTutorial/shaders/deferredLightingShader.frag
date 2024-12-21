#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform sampler2D gDepth;

struct Light {
	vec3 Position;
	vec3 Color;
	float Constant;
	float Linear;
	float Quadratic;
	float MaxDistance;
};

const int NR_LIGHTS = 2;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

const float near = 0.1f;
const float far = 100.0f;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float MaxSurroundingDepth(int searchRadius){
	float maxDepth = -1.0f;
	vec2 texelSize = 1.0f / textureSize(gDepth, 0).xy;

	for(int x = -searchRadius; x < searchRadius; x++){
		for(int y = -searchRadius; y < searchRadius; y++){
			vec2 coords = TexCoords + vec2(x, y) * texelSize;
			float currDepth = texture(gDepth, coords).r;
			maxDepth = currDepth > maxDepth ? currDepth : maxDepth;
		}
	}

	return maxDepth;
}

void main() {

	float currentDepth = texture(gDepth, TexCoords).r;
	float maxSurroundingDepth = MaxSurroundingDepth(2);

	currentDepth = LinearizeDepth(currentDepth);
	maxSurroundingDepth = LinearizeDepth(maxSurroundingDepth);

	if ((maxSurroundingDepth - currentDepth) > 0.1f){
		FragColor = vec4(vec3(0.0f), 1.0f);
	}
	else{
		FragColor = vec4(1.0f);
	}
	return;

//	FragColor = vec4(vec3(texture(gDepth, TexCoords).r), 1.0f);
//	return;
//	FragColor = vec4(vec3(texture(ssao, TexCoords).r), 1.0f);
//	return;
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;

	float Specular = texture(gAlbedoSpec, TexCoords).a;
	float AmbientOcclusion = texture(ssao, TexCoords).r;

	vec3 ambient = vec3(0.3f * Albedo * AmbientOcclusion);
	vec3 lighting = ambient;
	vec3 viewDir = normalize(-FragPos); // viewPos is at origin in view-space
	
	// iterate over all lights
	for(int i = 0; i < NR_LIGHTS; i++){
		float distance = length(lights[i].Position - FragPos);
//		if(distance > lights[i].MaxDistance){
//			continue;
//		}
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