#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;
uniform float exposure;

void main() {
	const float gamma = 2.2f;

	// FragColor = vec4(texture(hdrBuffer, TexCoords).rgb, 1.0f);
	// FragColor = vec4(pow(texture(hdrBuffer, TexCoords).rgb, vec3(1.0f/gamma)), 1.0f);
	// return;

	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBuffer, TexCoords).rgb;
	hdrColor += bloomColor;

	vec3 mapped = vec3(1.0f) - exp(-hdrColor * exposure); // hdrColor / (hdrColor + vec3(1.0f));
	// gamma correction
	mapped = pow(mapped, vec3(1.0f / gamma));

	FragColor = vec4(mapped, 1.0f);

}