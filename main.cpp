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

void NormalRenderPass(Window& window, FPSCamera& camera, ShaderLibrary& shaderLib, TextureBuffer3D& densityTextureBuffer, RenderVolume& renderVolume, Quad& floor, Quad& debugQuad, AdvancedQuad& displacementQuad, AdvancedQuad& displacementQuad2, float deltaTime);
void ShadowRenderPass(Window& window, ShaderLibrary& shaderLib, RenderVolume& renderVolume, Quad& floor, AdvancedQuad& displacementQuad, AdvancedQuad& displacementQuad2);

void PrintDisplacementSettings(int initialSteps, int refinementSteps);
void RenderCrosshair(Shader* shader, glm::vec3 color, GLuint width, GLuint height);

void DrawFullscreenQuad();

GLuint Create2DTexture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type);
GLuint CreateFramebuffer(GLuint colorTexture, GLuint depthTexture);
glm::mat4 GetShadowProjection();

// input values
bool wireframeMode = false;
bool showDebugQuad = false;
bool renderDensity = true;
bool renderMarchingCubes = true;
bool renderDisplacementQuad = true;
bool renderParticleSystem = true;
bool renderKDTree = false;

// shader keys
std::string debug3DTextureShaderKey = "debug-3dtexture";
std::string standardShaderKey = "standard";
std::string standardTextureShaderKey = "standard-texture";

std::string densityShaderKey = "density";
std::string geometryGeneratorKey = "geometry-generator";
std::string geometryRendererKey = "geometry-renderer";

std::string displacementShaderKey = "displacement";

std::string particleGeneratorKey = "particle-generator";
std::string particleRendererKey = "particle-renderer";

std::string hudKey = "hud";

std::string vsmShadowKey = "vsm-shadow";
std::string blurKey = "blur";
std::string shadowReceiverKey = "shadow-receiver";

// density texure info
GLuint densityWidth = 96;
GLuint densityHeight = 256;
GLuint densityDepth = 96;

GLfloat densityOffset = 0.0f;
GLfloat noiseStrength = 0.0f;
GLfloat densitySpeed = 10.0f;

GLuint densityVBO, densityVAO;

// particle system
std::vector<ParticleSystem> particleSystems;

// debug for 3d texture
GLuint debugLayer = 0;

// displacement values
bool useDisplacement = false;
int displacementInitialSteps = 10;
int displacementRefinementSteps = 5;

// lighting
glm::vec3 lightPos(15.0f, 20.0f, 15.0f);

// VSM
GLuint shadowMapTextureDepth;
GLuint shadowMapTexture;
GLuint shadowMapFBO;

GLuint blurTexture;
GLuint blurFBO;

static GLuint SHADOWMAP_SIZE = 1024;
static const float BLUR_SCALE = 1.5f;

int main()
{
	// setup window
	Window window(1280, 720, "Cascades Demo");

	// setup input
	Input* input = new Input();
	window.SetInput(input);

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
	shaderLib.WatchShader(debug3DTextureShaderKey);
	shaderLib.WatchShader(standardShaderKey);
	shaderLib.WatchShader(standardTextureShaderKey);

	// marching cubes shaders
	shaderLib.WatchShader(densityShaderKey);
	shaderLib.WatchShader(geometryGeneratorKey, { "outPosition", "outNormal" });
	shaderLib.WatchShader(geometryRendererKey);

	// displacement shaders
	shaderLib.WatchShader(displacementShaderKey);

	// particle shaders
	shaderLib.WatchShader(particleGeneratorKey, { "outPosition", "outVelocity", "outLifetime", "outType" });
	shaderLib.WatchShader(particleRendererKey);

	// hud shader
	shaderLib.WatchShader(hudKey);

	// vsm shader
	shaderLib.WatchShader(vsmShadowKey);
	shaderLib.WatchShader(blurKey);
	shaderLib.WatchShader(shadowReceiverKey);

	// compile all the shaders
	shaderLib.Update();

	// debug quad
	Quad debugQuad;

	// quad for displacement mapping test
	AdvancedQuad displacementQuad(glm::vec3(-4.5f, 4.0f, 4.0f), "Assets/Textures/rock.jpg", "Assets/Textures/rock_NRM.jpg", "Assets/Textures/rock_DISP.jpg");
	AdvancedQuad displacementQuad2(glm::vec3(-2.0f, 4.0f, 4.0f), "Assets/Textures/rock2.jpg", "Assets/Textures/rock2_NRM.jpg", "Assets/Textures/rock2_DISP.jpg");

	// floor
	Quad floor;

	FPSCamera camera(glm::vec3(0.0, 10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 5.0f, 0.2f);

	GLfloat currentFrameTime = glfwGetTime();
	GLfloat lastFrameTime = currentFrameTime;
	GLfloat deltaTime;

	// prerender density texture
	TextureBuffer3D densityTextureBuffer(densityWidth, densityDepth, densityHeight);
	// create render volume
	RenderVolume renderVolume(densityWidth, densityHeight, densityDepth);

	// shadow map
	shadowMapTextureDepth = Create2DTexture(GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_DEPTH_COMPONENT, GL_FLOAT);
	shadowMapTexture = Create2DTexture(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
	shadowMapFBO = CreateFramebuffer(shadowMapTexture, shadowMapTextureDepth);

	// blurring
	blurTexture = Create2DTexture(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
	blurFBO = CreateFramebuffer(blurTexture, -1);

	// last mouse state
	int lastMouseState = GLFW_RELEASE;

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);

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

		// debug debugLayer of 3d texture
		if (input->IsKeyDown(GLFW_KEY_RIGHT_BRACKET))
		{
			if (debugLayer < densityTextureBuffer.GetLayerCount())
				++debugLayer;
		}
		if (input->IsKeyDown(GLFW_KEY_SLASH))
		{
			if (debugLayer > 0)
				--debugLayer;
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

		// update shaders
		shaderLib.Update();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		ShadowRenderPass(window, shaderLib, renderVolume, floor, displacementQuad, displacementQuad2);
		NormalRenderPass(window, camera, shaderLib, densityTextureBuffer, renderVolume, floor, debugQuad, displacementQuad, displacementQuad2, deltaTime);

		// swap buffers
		window.SwapBuffers();

		// update input for next frame
		input->Update();
	}

	delete input;
	
	return 0;
}

void NormalRenderPass(Window& window, FPSCamera& camera, ShaderLibrary& shaderLib, TextureBuffer3D& densityTextureBuffer, RenderVolume& renderVolume, Quad& floor, Quad& debugQuad, AdvancedQuad& displacementQuad, AdvancedQuad& displacementQuad2, float deltaTime)
{
	window.SetClearColor({ 0.06f, 0.13f, 0.175f, 1.0f });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 projectionMatrix = camera.GetProjectionMatrix(static_cast<GLfloat>(window.GetWidth()), static_cast<GLfloat>(window.GetHeight()));
	glm::mat4 viewMatrix = camera.GetViewMatrix();

	// rendering

	// first renderpass -> render density texture
	if (renderDensity)
	{
		shaderLib.GetShader(densityShaderKey)->Use();

		glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "offset"), densityOffset);
		glUniform1f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "noiseStrength"), noiseStrength);
		glUniform3f(glGetUniformLocation(shaderLib.GetShader(densityShaderKey)->program, "dimensions"), densityWidth, densityDepth, densityHeight);

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
		Shader* textureShader = shaderLib.GetShader(standardTextureShaderKey);
		textureShader->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

		glUniformMatrix4fv(glGetUniformLocation(textureShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(textureShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(textureShader->program, "model"), 1, GL_FALSE, glm::value_ptr(glm::translate(debugQuad.GetModelMatrix(), glm::vec3(0.0f, 10.0f, 0.0f))));

		debugQuad.Render();

		glBindTexture(GL_TEXTURE_2D, 0);

		//Shader* debug3DShader = shaderLib.GetShader(debug3DTextureShaderKey);
		//debug3DShader->Use();

		//glActiveTexture(GL_TEXTURE0);
		//densityTextureBuffer.BindTexture();

		//glUniformMatrix4fv(glGetUniformLocation(debug3DShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//glUniformMatrix4fv(glGetUniformLocation(debug3DShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		//glUniformMatrix4fv(glGetUniformLocation(debug3DShader->program, "model"), 1, GL_FALSE, glm::value_ptr(debugQuad.GetModelMatrix()));

		//GLfloat layerCoord = static_cast<GLfloat>(debugLayer) / static_cast<GLfloat>(densityTextureBuffer.GetLayerCount());
		//glUniform1f(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "textureZCoord"), layerCoord);
		//glUniform1f(glGetUniformLocation(shaderLib.GetShader(debug3DTextureShaderKey)->program, "textureZCoord"), 0);

		//debugQuad.Render();
	}

	if (renderDisplacementQuad)
	{
		Shader* displacementShader = shaderLib.GetShader(displacementShaderKey);
		displacementShader->Use();

		glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

		glUniform3fv(glGetUniformLocation(displacementShader->program, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(displacementShader->program, "viewPos"), 1, &camera.GetPosition()[0]);

		glUniform1i(glGetUniformLocation(displacementShader->program, "useDisplacement"), useDisplacement);
		glUniform1i(glGetUniformLocation(displacementShader->program, "displacementInitialSteps"), displacementInitialSteps);
		glUniform1i(glGetUniformLocation(displacementShader->program, "displacementRefinementSteps"), displacementRefinementSteps);

		glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "model"), 1, GL_FALSE, glm::value_ptr(displacementQuad.GetModelMatrix()));
		displacementQuad.Render(displacementShader);

		glUniformMatrix4fv(glGetUniformLocation(displacementShader->program, "model"), 1, GL_FALSE, glm::value_ptr(displacementQuad2.GetModelMatrix()));
		displacementQuad2.Render(displacementShader);
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
		glUniform3fv(glGetUniformLocation(geometryRendererShader->program, "lightPos"), 1, &lightPos[0]);

		renderVolume.Render(geometryRendererShader);

		// render floor
		glm::mat4 floorMatrix;
		floorMatrix = glm::rotate(floorMatrix, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		floorMatrix = glm::scale(floorMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

		Shader* floorShader = shaderLib.GetShader(shadowReceiverKey);
		floorShader->Use();

		glUniformMatrix4fv(glGetUniformLocation(floorShader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(glGetUniformLocation(floorShader->program, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(floorShader->program, "model"), 1, GL_FALSE, glm::value_ptr(floorMatrix));


		glm::mat4 shadowProjection = GetShadowProjection();
		glUniformMatrix4fv(glGetUniformLocation(floorShader->program, "cameraToShadowProjection"), 1, GL_FALSE, glm::value_ptr(shadowProjection));

		glm::vec3 floorColor(0.0f, 0.36f, 0.04f);
		glUniform3fv(glGetUniformLocation(floorShader->program, "objectColor"), 1, &floorColor[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
			floor.Render();
		glBindTexture(GL_TEXTURE_2D, 0);
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
}

void ShadowRenderPass(Window& window, ShaderLibrary& shaderLib, RenderVolume& renderVolume, Quad& floor, AdvancedQuad& displacementQuad, AdvancedQuad& displacementQuad2)
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);

	window.SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* vsmShadowShader = shaderLib.GetShader(vsmShadowKey);
	vsmShadowShader->Use();

	glm::mat4 shadowProjection = GetShadowProjection();
	glUniformMatrix4fv(glGetUniformLocation(vsmShadowShader->program, "cameraToShadowProjection"), 1, GL_FALSE, glm::value_ptr(shadowProjection));

	glCullFace(GL_FRONT);

	if (renderDisplacementQuad)
	{
		glUniformMatrix4fv(glGetUniformLocation(vsmShadowShader->program, "model"), 1, GL_FALSE, glm::value_ptr(displacementQuad.GetModelMatrix()));
		displacementQuad.RenderShadowPass();

		glUniformMatrix4fv(glGetUniformLocation(vsmShadowShader->program, "model"), 1, GL_FALSE, glm::value_ptr(displacementQuad2.GetModelMatrix()));
		displacementQuad2.RenderShadowPass();
	}

	// render the render volume
	if (renderMarchingCubes)
	{
		glm::mat4 modelMatrix;
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(glGetUniformLocation(vsmShadowShader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

		renderVolume.RenderShadowPass();

		// render floor
		glm::mat4 floorMatrix;
		floorMatrix = glm::rotate(floorMatrix, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		floorMatrix = glm::scale(floorMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(glGetUniformLocation(vsmShadowShader->program, "model"), 1, GL_FALSE, glm::value_ptr(floorMatrix));

		floor.Render();
	}

	glCullFace(GL_BACK);

	// blur
	glDisable(GL_DEPTH_TEST);

	Shader* blurShader = shaderLib.GetShader(blurKey);
	blurShader->Use();

	// blur horizontally to blurTexture
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glUniform2f(glGetUniformLocation(blurShader->program, "scale"), 1.0f / SHADOWMAP_SIZE * BLUR_SCALE, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawFullscreenQuad();

	// blur texture vertically to shadowMapTexture
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurTexture);
	glUniform2f(glGetUniformLocation(blurShader->program, "scale"), 0.0f, 1.0f / SHADOWMAP_SIZE * BLUR_SCALE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawFullscreenQuad();

	glEnable(GL_DEPTH_TEST);

	// reset
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawFullscreenQuad()
{
	Quad quad;
	quad.Render();
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

GLuint Create2DTexture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

GLuint CreateFramebuffer(GLuint colorTexture, GLuint depthTexture)
{
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(colorTexture != -1)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	}

	if (depthTexture != -1)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	}

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(result != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR: Framebuffer is not complete!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

glm::mat4 GetShadowProjection()
{
	glm::mat4 shadowProjection;
	float orthoSize = 10.0f;
	shadowProjection *= glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 100.0f);
	shadowProjection *= glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	return shadowProjection;
}
