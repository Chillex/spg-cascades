#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "Color.h"
#include "Input.h"
#include "Shader.h"
#include "ShaderLibrary.h"
#include "Quad.h"
#include "Texture2D.h"
#include "FPSCamera.h"
#include "TextureBuffer3D.h"
#include "TextureBuffer.h"
#include "RenderVolume.h"

int main()
{
	// setup window
	Window window(1280, 720, "Cascades Demo");
	window.SetClearColor({ 0.06f, 0.13f, 0.175f, 1.0f });

	// setup input
	Input* input = new Input();
	window.SetInput(input);

	// input values
	bool wireframeMode = false;
	bool showDebugQuad = false;

	GLfloat densityPoints[] = {
		// Left bottom triangle
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		// Right top triangle
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f
	};

	GLuint densityVBO, densityVAO;
	glGenBuffers(1, &densityVBO);
	glGenVertexArrays(1, &densityVAO);
	glBindVertexArray(densityVAO);
	glBindBuffer(GL_ARRAY_BUFFER, densityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(densityPoints), &densityPoints, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glBindVertexArray(0);

	// Shader Library
	Path shaderPath(std::experimental::filesystem::current_path() / "Assets" / "Shaders");
	// the shader library will watch the given path with a thread
	// and hot reload all shaders added to it with "WatchShader()"
	ShaderLibrary shaderLib(shaderPath);
	
	std::string debug3DTextureShaderKey = "debug-3dtexture";
	std::string densityShaderKey = "density";
	std::string marchingCubesShaderKey = "marching-cubes";
	shaderLib.WatchShader(debug3DTextureShaderKey);
	shaderLib.WatchShader(densityShaderKey);
	shaderLib.WatchShader(marchingCubesShaderKey);

	// compile all the shaders
	shaderLib.Update();

	// debug for 3d texture
	Quad debugQuad;
	GLuint layer = 0;

	FPSCamera camera(glm::vec3(13.0f, 10.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 5.0f, 0.2f);

	GLfloat currentFrameTime = glfwGetTime();
	GLfloat lastFrameTime = currentFrameTime;
	GLfloat deltaTime;

	// prerender density texture
	GLuint width = 96;
	GLuint height = 256;
	GLuint depth = 96;
	//TextureBuffer densityTextureBuffer(96, 96);
	TextureBuffer3D densityTextureBuffer(width, depth, height);
	// create render volume
	RenderVolume renderVolume(width, height, depth);
	
	// variables for density
	GLfloat densityOffset = 0.0f;
	GLfloat noiseStrength = 0.3f;
	GLfloat densitySpeed = 10.0f;

	while(!window.ShouldClose())
	{
		// calculate delta time
		currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// check and call events
		glfwPollEvents();

		// handle input
		if(input->IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window.GetGLFWWindow(), GL_TRUE);
		}

		if (input->IsKeyPressed(GLFW_KEY_T))
			wireframeMode = !wireframeMode;

		// debug layer of 3d texture
		if (input->IsKeyDown(GLFW_KEY_RIGHT_BRACKET))
		{
			if (layer < densityTextureBuffer.GetLayerCount())
				++layer;
		}
		if (input->IsKeyDown(GLFW_KEY_SLASH))
		{
			if (layer > 0)
				--layer;
		}

		// change density offset
		if (input->IsKeyDown(GLFW_KEY_PAGE_UP))
			densityOffset += 1.0f * densitySpeed;
		if (input->IsKeyDown(GLFW_KEY_PAGE_DOWN))
			densityOffset -= 1.0f * densitySpeed;

		// change density speed
		if (input->IsKeyPressed(GLFW_KEY_M))
			++densitySpeed;
		if (input->IsKeyPressed(GLFW_KEY_N))
		{
			if(densitySpeed > 1.0f)
				--densitySpeed;
		}

		// change density speed
		if (input->IsKeyPressed(GLFW_KEY_K))
		{
			if (noiseStrength < 2.0f)
				noiseStrength += 0.05f;
		}
		if (input->IsKeyPressed(GLFW_KEY_J))
		{
			if (noiseStrength > 0.0f)
				noiseStrength -= 0.05f;
		}

		if (input->IsKeyPressed(GLFW_KEY_R))
			camera.PrintInfo();

		if (input->IsKeyPressed(GLFW_KEY_G))
			showDebugQuad = !showDebugQuad;

		// camera movement
		if (input->IsKeyDown(GLFW_KEY_W))
			camera.Move(FPSCamera::Movement::FORWARD, deltaTime);
		if (input->IsKeyDown(GLFW_KEY_S))
			camera.Move(FPSCamera::Movement::BACKWARD, deltaTime);
		if (input->IsKeyDown(GLFW_KEY_A))
			camera.Move(FPSCamera::Movement::RIGHT, deltaTime);
		if (input->IsKeyDown(GLFW_KEY_D))
			camera.Move(FPSCamera::Movement::LEFT, deltaTime);

		// camera look
		if(input->HasUnreadMouseChanges())
		{
			input->MouseChangesRead();
			camera.HandleMouseMovement(input->GetMouseOffsetX(), input->GetMouseOffsetY());
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// update shaders
		shaderLib.Update();

		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(static_cast<GLfloat>(window.GetWidth()), static_cast<GLfloat>(window.GetHeight()));
		glm::mat4 viewMatrix = camera.GetViewMatrix();

		// rendering

		// first renderpass -> render density texture
		shaderLib.GetShader(densityShaderKey)->Use();

		glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "offset"), densityOffset);
		glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "noiseStrength"), noiseStrength);
		glUniform3f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "dimensions"), width, depth, height);

		densityTextureBuffer.Bind();
		glBindVertexArray(densityVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, densityTextureBuffer.GetLayerCount());
		glBindVertexArray(0);
		densityTextureBuffer.Unbind();

		glViewport(0, 0, window.GetWidth(), window.GetHeight());

		// render debug quad
		if (showDebugQuad)
		{
			shaderLib.GetShader(debug3DTextureShaderKey)->Use();

			glActiveTexture(GL_TEXTURE0);
			densityTextureBuffer.BindTexture();
			
			glUniformMatrix4fv(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "model"), 1, GL_FALSE, glm::value_ptr(debugQuad.GetModelMatrix()));
			
			GLfloat layerCoord = static_cast<GLfloat>(layer) / static_cast<GLfloat>(densityTextureBuffer.GetLayerCount());
			glUniform1f(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "textureZCoord"), layerCoord);

			debugQuad.Render();
		}

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		// render the render volume
		glm::mat4 modelMatrix;
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

		Shader* marchingCubesShader = shaderLib.GetShader(marchingCubesShaderKey);
		marchingCubesShader->Use();

		glUniformMatrix4fv(glGetUniformLocation(marchingCubesShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(marchingCubesShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(marchingCubesShader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

		renderVolume.Render(densityTextureBuffer, marchingCubesShader);

		// swap buffers
		window.SwapBuffers();

		// update input for next frame
		input->Update();
	}

	delete input;
	
	return 0;
}
