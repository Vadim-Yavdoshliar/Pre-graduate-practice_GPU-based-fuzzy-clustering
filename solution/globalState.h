#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <string>

class globalState{

	static void loadBlob(const std::string filePath);

public:

static void initGraphics();
static void initWindow(USHORT width, USHORT height);
static void loadVertexShader(const std::string& filePath);
static void loadPixelShader(const std::string& filePath);
static void loadImage(const std::string& filePath);
static void createImageLikeBuffer();
static void addConstBuffer();
static void draw();
};

enum shaderType {
	VERTEX_SHADER,
	PIXEL_SHADER
};


