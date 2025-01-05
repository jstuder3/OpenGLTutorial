#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

const int KERNEL_SIZE = 9;

uniform sampler2D scene;
uniform vec2 offsets[KERNEL_SIZE];
uniform int edgeKernel[KERNEL_SIZE];
uniform float blurKernel[KERNEL_SIZE];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main() {
	FragColor = vec4(0.0f);
	vec3 sample[KERNEL_SIZE];

	if(chaos || shake) {
		for(int i = 0; i < KERNEL_SIZE; i++) {
			sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
		}
	}
	
	if(chaos) {
		for(int i = 0; i < KERNEL_SIZE; i++) {
			FragColor += vec4(sample[i] * edgeKernel[i], 0.0f);
		}
		FragColor.a = 1.0f;
	}
	else if (confuse) {
		FragColor = vec4(1.0f - texture(scene, TexCoords).rgb, 1.0f);
	}
	else if (shake) {
		for(int i = 0; i < KERNEL_SIZE; i++) {
			FragColor += vec4(sample[i] * blurKernel[i], 0.0f);
		}
		FragColor.a = 1.0f;
	}
	else {
		FragColor = texture(scene, TexCoords);
	}

}