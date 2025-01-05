#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height) : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false) {
	// init renderbuffer/framebuffer objects
	glGenFramebuffers(1, &this->MSFBO);
	glGenFramebuffers(1, &this->FBO);
	glGenRenderbuffers(1, &this->RBO);

	// init renderbuffer with multisampled color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for multisampled renderbuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // attach multisampled render buffer object to framebuffer
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO! (" << __FILE__ ": Line " << __LINE__ << ")" << std::endl;
		exit(1);
	}

	// bind texture / render target to fbo
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	this->Texture.Generate(width, height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); // attach texture to framebuffer as color attachment
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::POSTPROCESSOR: Failed to initialize FBO! (" << __FILE__ ": Line " << __LINE__ << ")" << std::endl;
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->initRenderData();

	this->PostProcessingShader.SetInteger("scene", 0, true);
	float offset = 1.0f / 300.0f;
	float offsets[9][2] = {
		{ -offset,  offset  },  // top-left
		{  0.0f,    offset  },  // top-center
		{  offset,  offset  },  // top-right
		{ -offset,  0.0f    },  // center-left
		{  0.0f,    0.0f    },  // center-center
		{  offset,  0.0f    },  // center - right
		{ -offset, -offset  },  // bottom-left
		{  0.0f,   -offset  },  // bottom-center
		{  offset, -offset  }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);

	int edgeKernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edgeKernel"), 9, edgeKernel);

	float blurKernel[9] = {
		 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};

	glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blurKernel"), 9, blurKernel);
}

void PostProcessor::initRenderData() {
	unsigned int VBO;
	float vertices[] = {
		// pos        // tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void PostProcessor::BeginRender() {
	// set up buffer that regular rendering should output to (in our case: multisampling buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender() {
	// copy result from multisampling framebuffer to regular framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::Render(float time) {
	// set uniforms of post-processing shader
	this->PostProcessingShader.Use();
	this->PostProcessingShader.SetFloat("time", time);
	this->PostProcessingShader.SetInteger("confuse", this->Confuse);
	this->PostProcessingShader.SetInteger("chaos", this->Chaos);
	this->PostProcessingShader.SetInteger("shake", this->Shake);

	// bind pre-rendered scene texture
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();

	// draw to screen quad
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
