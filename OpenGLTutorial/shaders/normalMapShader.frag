#version 330 core
out vec4 FragColor;

in VertexData {
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
} InData;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main(){
	vec3 normal = texture(normalMap, InData.TexCoords).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(InData.TBN * normal);

	//vec3 normal = vec3(0.0, 0.0, 1.0);

	vec3 ambient = texture(diffuseMap, InData.TexCoords).rgb * 0.05f;

	vec3 lightDirection = normalize(lightPos - InData.FragPos);
	vec3 diffuse = dot(lightDirection, normal) * texture(diffuseMap, InData.TexCoords).rgb * 0.5f;

	vec3 viewDirection = normalize(viewPos - InData.FragPos);
	vec3 halfwayVector = normalize(lightDirection + viewDirection);
	vec3 specular = pow(max(0.0f, dot(halfwayVector, normal)), 512.0f) * texture(diffuseMap, InData.TexCoords).rgb * 1.0f;

	FragColor = vec4(ambient + diffuse + specular, 1.0);

	//FragColor = vec4(1.0f);
//	vec3 normal = texture(normalMap, InData.TexCoords).rgb;
//	normal = normalize(normal * 2.0 - 1.0);
	
	// FragColor = vec4(texture(diffuseMap, InData.TexCoords).rgb, 1.0f);
}