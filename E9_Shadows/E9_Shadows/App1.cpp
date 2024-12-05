// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// Create sphere mesh
	lightMesh0 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	lightMesh1 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	shadowDataMesh0 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2, -screenWidth / 2, screenHeight / 2);
	shadowDataMesh1 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2, -screenWidth / 2, screenHeight / 2);

	// initial shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMaps[0] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMaps[1] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure directional light
	lights[0] = new Light();
	lights[0]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	lights[0]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	lights[0]->setDirection(0.0f, -0.7f, 0.7f);
	lights[0]->setPosition(0.f, 0.f, -10.f);
	lights[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	light0Pos[0] = lights[0]->getPosition().x; light0Pos[1] = lights[0]->getPosition().y; light0Pos[2] = lights[0]->getPosition().z;
	light0Dir[0] = lights[0]->getDirection().x; light0Dir[1] = lights[0]->getDirection().y; light0Dir[2] = lights[0]->getDirection().z;

	// Configure directional light
	lights[1] = new Light();
	lights[1]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	lights[1]->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	lights[1]->setDirection(0.0f, -0.7f, -0.7f);
	lights[1]->setPosition(0.f, 0.f, 10.f);
	lights[1]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	light1Pos[0] = lights[1]->getPosition().x; light1Pos[1] = lights[1]->getPosition().y; light1Pos[2] = lights[1]->getPosition().z;
	light1Dir[0] = lights[1]->getDirection().x; light1Dir[1] = lights[1]->getDirection().y; light1Dir[2] = lights[1]->getDirection().z;

}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	// Perform depth pass
	depthPass();
	// Render scene
	finalPass();

	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMaps[0]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = lights[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();

	// ---------- Shadow Map 1 ---------- //

	// Set the render target to be the render to texture.
	shadowMaps[1]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights[1]->generateViewMatrix();
	XMMATRIX lightViewMatrix1 = lights[1]->getViewMatrix();
	XMMATRIX lightProjectionMatrix1 = lights[1]->getOrthoMatrix();
	worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix1, lightProjectionMatrix1);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	lights[0]->setPosition(light0Pos[0], light0Pos[1], light0Pos[2]);
	lights[0]->setDirection(light0Dir[0], light0Dir[1], light0Dir[2]);

	lights[1]->setPosition(light1Pos[0], light1Pos[1], light1Pos[2]);
	lights[1]->setDirection(light1Dir[0], light1Dir[1], light1Dir[2]);

	depthMaps[0] = shadowMaps[0]->getDepthMapSRV();
	depthMaps[1] = shadowMaps[1]->getDepthMapSRV();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, 
		textureMgr->getTexture(L"brick"), depthMaps, lights);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), depthMaps, lights);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Render the light mesh 0 with the texture shader applied
	lightMesh0->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(lights[0]->getPosition().x, lights[0]->getPosition().y, lights[0]->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L""));
	textureShader->render(renderer->getDeviceContext(), lightMesh0->getIndexCount());

	// Render the light mesh 1 with the texture shader applied
	lightMesh1->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(lights[1]->getPosition().x, lights[1]->getPosition().y, lights[1]->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L""));
	textureShader->render(renderer->getDeviceContext(), lightMesh1->getIndexCount());

	// RENDER THE TEXTURE SCENE
	renderer->setZBuffer(false);
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	shadowDataMesh0->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, depthMaps[0]);
	textureShader->render(renderer->getDeviceContext(), shadowDataMesh0->getIndexCount());

	shadowDataMesh1->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix * XMMatrixTranslation(500, 0, 0), orthoViewMatrix, orthoMatrix, depthMaps[1]);
	textureShader->render(renderer->getDeviceContext(), shadowDataMesh1->getIndexCount());
	renderer->setZBuffer(true);
	gui();
	renderer->endScene();
}



void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Lights UI
	ImGui::Text("Light 0 Properties");
	ImGui::SliderFloat3("Light 0 Position", light0Pos, -25.0f, 125.0f, "%.f");
	ImGui::SliderFloat3("Light 0 Direction", light0Dir, -1.0f, 1.0f, "%.1f");

	ImGui::Text("Light 1 Properties");
	ImGui::SliderFloat3("Light 1 Position", light1Pos, -25.0f, 125.0f, "%.f");
	ImGui::SliderFloat3("Light 1 Direction", light1Dir, -1.0f, 1.0f, "%.1f");
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

