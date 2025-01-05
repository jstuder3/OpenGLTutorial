#pragma once
#include "Shader.h"
#include "Texture.h"

class PostProcessor
{
public:
	// basic state
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	// possible post-processing effects
	bool Confuse, Chaos, Shake;

	PostProcessor(Shader shader, unsigned int width, unsigned int height);

	// init framebuffers
	void BeginRender();

	// call after rendering a frame to store the rendered data into a texture object
	void EndRender();

	// render post-processing effects to screen quad
	void Render(float time);
private:
	// multisampling framebuffer, regular framebuffer
	unsigned int MSFBO, FBO;

	// multisampling color buffer
	unsigned int RBO;
	// screen quad
	unsigned int VAO;

	// initialize screen quad
	void initRenderData();
};
