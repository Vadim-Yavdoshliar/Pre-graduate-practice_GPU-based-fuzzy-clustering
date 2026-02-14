#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <string>
#include <unordered_map>
#include <exception>

class globalState{

public:

static void initGraphics();
static void initWindow(USHORT width, USHORT height);

static void loadVertexShader(USHORT index, const std::string& filePath);
static void loadPixelShader(USHORT index, const std::string& filePath);

static void setVertexShader(USHORT index);
static void setPixelShader(USHORT index);

static void loadImage(USHORT index,const std::string& filePath);
static void removeImage(USHORT index);

static void createImageLikeBuffer();
static void createConstBuffer();

static void processState();
static void draw();

static void freeResources();

private:

	struct state_data {

		USHORT windowWidth = 0u, windowHeight = 0u;

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;

		std::unordered_map<USHORT, ComPtr<ID3D11VertexShader>> vertexShaders;
		std::unordered_map<USHORT, ComPtr<ID3D11PixelShader>> pixelShaders;

	};

	static state_data data;

	static LRESULT windowProcedure(HWND wnd, UINT msg,WPARAM wParam, LPARAM lParam);

};



