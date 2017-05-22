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
#include "AdvancedQuad.h"
#include "ParticleSystem.h"
#include "KDTreeController.h"
#include "Ray.h"

void PrintDisplacementSettings(int initialSteps, int refinementSteps);
void RenderCrosshair(Shader* shader, glm::vec3 color, GLuint width, GLuint height);

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
	bool renderDensity = true;
	bool renderMarchingCubes = true;
	bool renderDisplacementQuad = false;
	bool renderParticleSystem = true;
	bool renderKDTree = false;

	// lighting
	glm::vec3 lightPos(0.0f, 10.0f, 10.0f);

	// displacement values
	bool useDisplacement = false;
	int displacementInitialSteps = 10;
	int displacementRefinementSteps = 5;

	PrintDisplacementSettings(displacementInitialSteps, displacementRefinementSteps);

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

	// debugging shaders
	std::string debug3DTextureShaderKey = "debug-3dtexture";
	shaderLib.WatchShader(debug3DTextureShaderKey);
	std::string standardShaderKey = "standard";
	shaderLib.WatchShader(standardShaderKey);

	// marching cubes shaders
	std::string densityShaderKey = "density";
	shaderLib.WatchShader(densityShaderKey);
	std::string geometryGeneratorKey = "geometry-generator";
	shaderLib.WatchShader(geometryGeneratorKey, { "outPosition", "outNormal" });
	std::string geometryRendererKey = "geometry-renderer";
	shaderLib.WatchShader(geometryRendererKey);

	// displacement shaders
	std::string displacementShaderKey = "displacement";
	shaderLib.WatchShader(displacementShaderKey);

	// particle shaders
	std::string particleGeneratorKey = "particle-generator";
	shaderLib.WatchShader(particleGeneratorKey, { "outPosition", "outVelocity", "outLifetime", "outType" });
	std::string particleRendererKey = "particle-renderer";
	shaderLib.WatchShader(particleRendererKey);

	// hud shader
	std::string hudKey = "hud";
	shaderLib.WatchShader(hudKey);

	// compile all the shaders
	shaderLib.Update();

	// quad for displacement mapping test
	AdvancedQuad displacementQuad(glm::vec3(0.0f), "Assets/Textures/rock2.jpg", "Assets/Textures/rock2_NRM.jpg", "Assets/Textures/rock2_DISP.jpg");

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
	TextureBuffer3D densityTextureBuffer(width, depth, height);
	// create render volume
	RenderVolume renderVolume(width, height, depth);
	
	// variables for density
	GLfloat densityOffset = 0.0f;
	GLfloat noiseStrength = 0.0f;
	GLfloat densitySpeed = 10.0f;

	// particle system
	std::vector<ParticleSystem> particleSystems;

	// last mouse state
	int lastMouseState = GLFW_RELEASE;

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
		{
			densityOffset += 1.0f * densitySpeed;
			renderDensity = true;
		}
		if (input->IsKeyDown(GLFW_KEY_PAGE_DOWN))
		{
			densityOffset -= 1.0f * densitySpeed;
			renderDensity = true;
		}

		// change density speed
		if (input->IsKeyPressed(GLFW_KEY_O))
			++densitySpeed;
		if (input->IsKeyPressed(GLFW_KEY_I))
		{
			if(densitySpeed > 1.0f)
				--densitySpeed;
		}

		// change density speed
		if (input->IsKeyPressed(GLFW_KEY_K))
		{
			if (noiseStrength < 2.0f)
				noiseStrength += 0.05f;

			renderDensity = true;
		}
		if (input->IsKeyPressed(GLFW_KEY_J))
		{
			if (noiseStrength > 0.0f)
				noiseStrength -= 0.05f;

			renderDensity = true;
		}

		if (input->IsKeyPressed(GLFW_KEY_R))
			camera.PrintInfo();

		// toggle rendering
		if (input->IsKeyPressed(GLFW_KEY_G))
			showDebugQuad = !showDebugQuad;

		if (input->IsKeyPressed(GLFW_KEY_C))
			useDisplacement = !useDisplacement;

		if (input->IsKeyPressed(GLFW_KEY_M))
			renderMarchingCubes = !renderMarchingCubes;

		if (input->IsKeyPressed(GLFW_KEY_B))
			renderDisplacementQuad = !renderDisplacementQuad;

		if (input->IsKeyPressed(GLFW_KEY_F))
			renderKDTree = !renderKDTree;

		// change displacement
		if (input->IsKeyPressed(GLFW_KEY_7))
		{
			if (displacementInitialSteps > 0)
				--displacementInitialSteps;

			PrintDisplacementSettings(displacementInitialSteps, displacementRefinementSteps);
		}
		if (input->IsKeyPressed(GLFW_KEY_8))
		{
			++displacementInitialSteps;

			PrintDisplacementSettings(displacementInitialSteps, displacementRefinementSteps);
		}
		if (input->IsKeyPressed(GLFW_KEY_9))
		{
			if (displacementRefinementSteps > 0)
				--displacementRefinementSteps;

			PrintDisplacementSettings(displacementInitialSteps, displacementRefinementSteps);
		}
		if (input->IsKeyPressed(GLFW_KEY_0))
		{
			++displacementRefinementSteps;

			PrintDisplacementSettings(displacementInitialSteps, displacementRefinementSteps);
		}

		// clear particle systems
		if (input->IsKeyPressed(GLFW_KEY_X))
		{
			particleSystems.clear();
		}

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

		// click events
		int leftMouseButtonState = glfwGetMouseButton(window.GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT);
		if (leftMouseButtonState == GLFW_RELEASE && lastMouseState == GLFW_PRESS)
		{
			if(renderVolume.kdRoot != nullptr)
			{
				Ray ray(camera.GetPosition(), camera.GetDirection());
				std::vector<std::pair<Triangle, float>> hitTriangles;
				
				KDTreeController::CheckHit(ray, renderVolume.kdRoot, hitTriangles, renderVolume.triangles);
				
				if (hitTriangles.size() > 0)
				{
					std::pair<Triangle, float>& nearestTri = *std::min_element(hitTriangles.begin(), hitTriangles.end(), [](const auto& a, const auto& b)
					{
						return a.second < b.second;
					});

					glm::vec3 hitPoint = camera.GetPosition() + camera.GetDirection() * nearestTri.second;
					particleSystems.push_back(ParticleSystem(hitPoint, 0.2f));
				}
			}
		}
		lastMouseState = leftMouseButtonState;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// update shaders
		shaderLib.Update();

		glm::mat4 projectionMatrix = camera.GetProjectionMatrix(static_cast<GLfloat>(window.GetWidth()), static_cast<GLfloat>(window.GetHeight()));
		glm::mat4 viewMatrix = camera.GetViewMatrix();

		// rendering

		// first renderpass -> render density texture
		if(renderDensity)
		{
			shaderLib.GetShader(densityShaderKey)->Use();

			glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "offset"), densityOffset);
			glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "noiseStrength"), noiseStrength);
			glUniform3f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "dimensions"), width, depth, height);

			densityTextureBuffer.Bind();
			glBindVertexArray(densityVAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, densityTextureBuffer.GetLayerCount());
			glBindVertexArray(0);
			densityTextureBuffer.Unbind();

			renderDensity = false;

			// generate geometry once
			renderVolume.GenerateGeometry(densityTextureBuffer, shaderLib.GetShader(geometryGeneratorKey));
		}

		glViewport(0, 0, window.GetWidth(), window.GetHeight());

		if (renderParticleSystem)
		{
			for (size_t p = 0; p < particleSystems.size(); ++p)
			{
				// update particles
				particleSystems[p].UpdateParticles(shaderLib.GetShader(particleGeneratorKey), deltaTime);

				// render particles
				Shader* particleRenderShader = shaderLib.GetShader(particleRendererKey);

				particleRenderShader->Use();

				glm::mat4 modelMatrix;
				glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
				glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
				glUniformMatrix4fv(glGetUniformLocation(particleRenderShader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
				particleSystems[p].Render(particleRenderShader);
			}
		}

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

		if (renderDisplacementQuad)
		{
			Shader* displacementShader = shaderLib.GetShader(displacementShaderKey);
			displacementShader->Use();

			glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "model"), 1, GL_FALSE, glm::value_ptr(displacementQuad.GetModelMatrix()));

			glUniform3fv(glGetUniformLocation(displacementShader->program, "lightPos"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(displacementShader->program, "viewPos"), 1, &camera.GetPosition()[0]);

			glUniform1i(glGetUniformLocation(displacementShader->program, "useDisplacement"), useDisplacement);
			glUniform1i(glGetUniformLocation(displacementShader->program, "displacementInitialSteps"), displacementInitialSteps);
			glUniform1i(glGetUniformLocation(displacementShader->program, "displacementRefinementSteps"), displacementRefinementSteps);

			displacementQuad.Render(displacementShader);
		}

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		// render the render volume
		if (renderMarchingCubes)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

			Shader* geometryRendererShader = shaderLib.GetShader(geometryRendererKey);
			geometryRendererShader->Use();

			glUniformMatrix4fv(glGetUniformLocation(geometryRendererShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(glGetUniformLocation(geometryRendererShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(glGetUniformLocation(geometryRendererShader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glUniform3fv(glGetUniformLocation(geometryRendererShader->program, "cameraPos"), 1, glm::value_ptr(camera.GetPosition()));
			glUniform3fv(glGetUniformLocation(geometryRendererShader->program, "lightPos"), 1, glm::value_ptr(lightPos));

			renderVolume.Render(geometryRendererShader);
		}

		if (renderKDTree)
		{
			Shader* standardShader = shaderLib.GetShader(standardShaderKey);
			standardShader->Use();

			glUniformMatrix4fv(glGetUniformLocation(standardShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(glGetUniformLocation(standardShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

			renderVolume.kdRoot->Draw(standardShader);
		}

		// render crosshair
		Shader* hudShader = shaderLib.GetShader(hudKey);
		hudShader->Use();

		glm::mat4 hudProjection = glm::ortho(0.0f, static_cast<GLfloat>(window.GetWidth()), 0.0f, static_cast<GLfloat>(window.GetHeight()));
		glUniformMatrix4fv(glGetUniformLocation(hudShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(hudProjection));
		RenderCrosshair(hudShader, glm::vec3(1.0f), window.GetWidth(), window.GetHeight());

		// swap buffers
		window.SwapBuffers();

		// update input for next frame
		input->Update();
	}

	delete input;
	
	return 0;
}

void PrintDisplacementSettings(int initialSteps, int refinementSteps)
{
	printf("Displacement Info:\n===================\nInitial Steps: %d\nRefinementSteps: %d\n\n", initialSteps, refinementSteps);
}

GLuint crosshairVAO = 0;
GLuint crosshairVBO;
void RenderCrosshair(Shader* shader, glm::vec3 color, GLuint width, GLuint height)
{
	if (crosshairVAO == 0)
	{
		GLint crosshairVertices[] = {
			width / 2 - 7, height / 2, // horizontal line
			width / 2 + 7, height / 2,

			width / 2, height / 2 + 7, //vertical line
			width / 2, height / 2 - 7
		};
		// Setup plane VAO
		glGenVertexArrays(1, &crosshairVAO);
		glGenBuffers(1, &crosshairVBO);
		glBindVertexArray(crosshairVAO);
		glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), &crosshairVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), static_cast<GLvoid*>(0));
	}

	glBindVertexArray(crosshairVAO);

	GLint uObjectColor = glGetUniformLocation(shader->program, "objectColor");
	glUniform3f(uObjectColor, color.x, color.y, color.z);

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);

	glBindVertexArray(0);
}
