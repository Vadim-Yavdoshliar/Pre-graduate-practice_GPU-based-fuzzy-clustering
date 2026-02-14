#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

#include <string>
#include <unordered_map>
#include <exception>
#include <iostream>

class globalState{

public:

static void initGraphics();
static void initWindow(USHORT width, USHORT height);
static void initDrawingSurface();

static bool windowIsOpen();

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

	static bool checkCompleteInit();
	static void resetState();

	struct state_data {

		USHORT windowWidth = 0u, windowHeight = 0u;
		bool windowWorks = false;

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGIFactory2> factory;
		ComPtr<IDXGISwapChain1> swapchain;
		ComPtr<ID3D11Query> query;

		ComPtr<ID3D11RenderTargetView> target;

		std::unordered_map<USHORT, ComPtr<ID3D11VertexShader>> vertexShaders;
		std::unordered_map<USHORT, ComPtr<ID3D11PixelShader>> pixelShaders;

		HWND wndPtr;
		FLOAT clearColor[4] = {0.0f,0.0f,1.0f,1.0f};
		const wchar_t* className = L"GRAPHICS_WINDOW";
	};

	static state_data data;

	static LRESULT windowProcedure(HWND wnd, UINT msg,WPARAM wParam, LPARAM lParam);

};



