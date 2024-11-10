#version 330 core
out vec4 FragColor;

in VertexData{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} InData;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;


vec3 lightDir;
vec3 normal;


float ShadowCalculation(vec4 fragPosLightSpace){

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords + 1.0) / 2.0f;

	if(projCoords.z > 1.0){
		return 0.0f;
	}

	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
	float currentDepth = projCoords.z;
	float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);

	for(int x = -1; x <= 1; x++){
		for(int y = -1; y <= 1; y++){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
		}
	}

	shadow /= 9.0f;

	// float closestDepth = texture(shadowMap, projCoords.xy).r;
	// float shadow = (currentDepth - bias) > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

void main(){
	vec3 color = texture(diffuseTexture, InData.TexCoords).rgb;
	vec3 lightColor = vec3(1.0f);
	normal = normalize(InData.Normal);

	//ambient
	vec3 ambient = 0.15f * lightColor;

	//diffuse
	lightDir = normalize(lightPos - InData.FragPos);
	float diffuseStrength = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diffuseStrength * lightColor;

	//specular
	vec3 viewDir = normalize(viewPos - InData.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularStrength = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
	vec3 specular = specularStrength * lightColor;

	// shadow calculation
	float shadow = ShadowCalculation(InData.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;

	FragColor = vec4(lighting, 1.0f);


}

