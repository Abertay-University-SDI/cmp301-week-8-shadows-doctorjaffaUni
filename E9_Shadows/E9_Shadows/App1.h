// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;

	SphereMesh* lightMesh0;
	SphereMesh* lightMesh1;

	Light* lights[2];
	AModel* model;
	ShadowShader* shadowShader;
	DepthShader* depthShader;

	ShadowMap* shadowMaps[2];

	ID3D11ShaderResourceView* depthMaps[2];

	// Light Variables
	float light0Dir[3];
	float light1Dir[3];

	float light0Pos[3];
	float light1Pos[3];
};

#endif