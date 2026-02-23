#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

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

static void loadVertexShader(USHORT index, const std::wstring& filePath);
static void loadPixelShader(USHORT index, const std::wstring& filePath);

static void setVertexShader(USHORT index);
static void setPixelShader(USHORT index);

static void loadImage(USHORT index,const std::string& filePath);
static void removeImage(USHORT index);

static void setImage(USHORT index);

static void createImageLikeBuffer();
static void createConstBuffer();

static void processState();
static void draw(UINT imageIndex);

static void freeResources();

private:

	static bool checkCompleteInit();
	static void resetState();

	struct imageReference {
		UINT width = 0u, height = 0u;
		ComPtr<ID3D11Texture2D> texture;
		ComPtr<ID3D11ShaderResourceView> resourceView;
		ComPtr<ID3D11Buffer> vertexBuf;
	};

	struct stateData {

		USHORT windowWidth = 0u, windowHeight = 0u;
		bool windowWorks = false;

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGIFactory2> factory;
		ComPtr<IDXGISwapChain1> swapchain;
		ComPtr<ID3D11Query> query;
		ComPtr<ID3D11SamplerState> sampler;

		ComPtr<ID3D11RenderTargetView> target;

		std::unordered_map<USHORT, ComPtr<ID3D11VertexShader>> vertexShaders;
		std::unordered_map<USHORT, ComPtr<ID3D11PixelShader>> pixelShaders;
		ComPtr<ID3D11InputLayout> inputLayout;
		std::unordered_map<USHORT, imageReference> images;

		HWND wndPtr = nullptr;
		FLOAT clearColor[4] = {0.2f,0.5f,0.5f,1.0f};
		const wchar_t* className = L"GRAPHICS_WINDOW";
	};

	static stateData data;

	static LRESULT windowProcedure(HWND wnd, UINT msg,WPARAM wParam, LPARAM lParam);

};



