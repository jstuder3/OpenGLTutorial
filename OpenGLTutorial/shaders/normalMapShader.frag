#version 330 core
out vec4 FragColor;

in VertexData {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} InData;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main(){

	vec3 fragPos = InData.TangentFragPos;
	vec3 lightPos = InData.TangentLightPos;
	vec3 viewPos = InData.TangentViewPos;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec2 texCoords = ParallaxMapping(InData.TexCoords, viewDir);
    // vec2 texCoords = InData.TexCoords;

	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0){
		discard;
	}


	vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normal * 2.0 - 1.0;
//	normal = InData.TBN * normal; // convert from tangent to world space
	normal = normalize(normal);
//	normal = vec3(0, 0, 1);
//	FragColor = vec4(normal, 1.0);

	vec3 ambient = texture(diffuseMap, texCoords).rgb * 0.05f;

	vec3 lightDirection = normalize(lightPos - fragPos);
	vec3 diffuse = max(dot(lightDirection, normal), 0.0f) * texture(diffuseMap, texCoords).rgb * 0.5f;

	vec3 viewDirection = normalize(viewPos - fragPos);
	vec3 halfwayVector = normalize(lightDirection + viewDirection);
	vec3 specular = pow(max(0.0f, dot(halfwayVector, normal)), 512.0f) * texture(diffuseMap, texCoords).rgb * 1.0f;

	FragColor = vec4(ambient + diffuse + specular, 1.0);

////	FragColor = vec4(1.0f);
//////	vec3 normal = texture(normalMap, InData.TexCoords).rgb;
////	normal = normalize(normal * 2.0 - 1.0);
////	
////	 FragColor = vec4(texture(diffuseMap, InData.TexCoords).rgb, 1.0f);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
	const float minLayers = 256;
	const float maxLayers = 1024;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0f, 0.0f, 1.0f), viewDir), 0.0f));
	
	float layerDepth = 1.0 / numLayers;

	float currentLayerDepth = 0.0;
	vec2 P = viewDir.xy / viewDir.z * heightScale;
	vec2 deltaTexCoords = P / numLayers;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue){
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

	return finalTexCoords;


//	float height = texture(depthMap, texCoords).r;
//	vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
//	return texCoords - p;
}