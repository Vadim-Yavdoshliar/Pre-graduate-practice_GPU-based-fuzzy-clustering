

#include "globalState.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

globalState::stateData globalState::data;

void globalState::initGraphics()
{
	auto hr = D3D11CreateDevice
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED |
		D3D11_CREATE_DEVICE_BGRA_SUPPORT |
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0u,
		D3D11_SDK_VERSION,
		&data.device,
		NULL,
		&data.context
	);

	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nBasic graphics resources creation failed.\n\n");
	}

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)data.factory.GetAddressOf());
	
	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nBasic(2) graphics resources creation failed.\n\n");
	}

	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.MiscFlags = 0u;
	queryDesc.Query = D3D11_QUERY_EVENT;
	hr = data.device->CreateQuery(&queryDesc, &data.query);

	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nBasic(3) graphics resources creation failed.\n\n");
	}

	data.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	D3D11_VIEWPORT vp;
	vp.Width = data.windowWidth;
	vp.Height = data.windowHeight;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	data.context->RSSetViewports(1, &vp);

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = data.device->CreateSamplerState(
		&samplerDesc,
		&data.sampler);

	if (hr != S_OK) {
		throw std::exception("\n\n\tSampler creation failed");
	}
	
	data.context->PSSetSamplers(
		0,
		1,
		data.sampler.GetAddressOf());


	ComPtr<ID3D11BlendState> blendStateNoBlend;

	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = data.device->CreateBlendState(&BlendState, &blendStateNoBlend);
	if (hr != S_OK) {
		throw std::exception("Blend state creation failed");
	}
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	data.context->OMSetBlendState(blendStateNoBlend.Get(), blendFactor, sampleMask);
}


LRESULT globalState::windowProcedure
(
	HWND wnd, 
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{

	if (msg == WM_DESTROY)
	{
		data.windowWorks = false;
	}

	return DefWindowProcW(wnd, msg, wParam, lParam);
}


void globalState::initWindow
(
	USHORT width,
	USHORT height
)
{


	WNDCLASSW desc = {};
	desc.lpfnWndProc = globalState::windowProcedure;
	desc.lpszClassName = data.className;
	desc.hCursor = NULL;
	desc.hInstance = GetModuleHandleW(NULL);
	desc.cbClsExtra = NULL;
	desc.cbWndExtra = NULL;
	desc.hbrBackground = NULL;
	desc.hIcon = NULL;
	desc.lpszMenuName = NULL;
	desc.style = NULL;
	RegisterClassW(&desc);

	USHORT screenWidth = (USHORT)GetSystemMetrics(SM_CXSCREEN);
	USHORT screenHeight = (USHORT)GetSystemMetrics(SM_CYSCREEN);

	width = (width > screenWidth) ? 700u : width;
	height = (height > screenHeight) ? 700u : height;

	data.windowWidth = width;
	data.windowHeight = height;

	data.wndPtr = CreateWindowExW
	(
		NULL,
		data.className,
		L"Output window",
		WS_CAPTION | WS_SYSMENU,
		(screenWidth - width)/2u,
		(screenHeight - height)/2u,
		width,
		height,
		NULL,
		NULL,
		GetModuleHandleW(NULL),
		NULL
	);

	if (data.wndPtr == nullptr)
	{
		resetState();
		throw std::exception("\n\nWindow creation failed");
	}

	ShowWindow(data.wndPtr, SW_SHOW);

	data.windowWorks = true;

}

void globalState::initDrawingSurface()
{
	if (data.device == nullptr || data.wndPtr == nullptr) {
		resetState();
		throw std::exception("\n\nDrawing surface cannot be created unless graphics resources and window are initialized\n\n");
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2u;
	desc.SampleDesc.Count = 1u;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.Width = data.windowWidth;
	desc.Height = data.windowHeight;

	auto hr = data.factory->CreateSwapChainForHwnd
	(
		data.device.Get(),
		data.wndPtr,
		&desc,
		NULL,
		NULL,
		&data.swapchain
	);

	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nDrawing surface creation failed\n\n");
	}

	


}

bool globalState::windowIsOpen()
{
	return data.windowWorks;
}

void globalState::loadVertexShader(USHORT index, const std::wstring& filePath)
{
	if (data.device == nullptr)
	{
		throw std::exception("\n\n\tException : Attempt to create a shader before graphics is initialized");
	}
	ComPtr<ID3DBlob> blob;
	auto hr = D3DReadFileToBlob(filePath.c_str(), &blob);
	if (hr != S_OK)
	{
		std::cout << "\n\tVertex shader loading failed. Incorrect file path specified or general internal error occured.";
		return;
	}
	ComPtr<ID3D11VertexShader> shader;
	hr = data.device->CreateVertexShader
	(
		blob->GetBufferPointer(), 
		blob->GetBufferSize(),
		nullptr,
		&shader
	);
	if (hr != S_OK)
	{
		std::cout << "\n\tVertex shader loading failed. Loaded shader isn't a shader or corrupted. Internal error occured.";
		return;
	}

	D3D11_INPUT_ELEMENT_DESC vertexPosInputDecs = {};
	vertexPosInputDecs.SemanticName = "POSITION";
	vertexPosInputDecs.SemanticIndex = 0u;
	vertexPosInputDecs.Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexPosInputDecs.InputSlot = 0u;
	vertexPosInputDecs.AlignedByteOffset = 0u;
	vertexPosInputDecs.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexPosInputDecs.InstanceDataStepRate = 0u;

	D3D11_INPUT_ELEMENT_DESC vertexTexInputDecs = {};
	vertexTexInputDecs.SemanticName = "UVCOORD";
	vertexTexInputDecs.SemanticIndex = 0u;
	vertexTexInputDecs.Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexTexInputDecs.InputSlot = 0u;
	vertexTexInputDecs.AlignedByteOffset = 8u;
	vertexTexInputDecs.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexTexInputDecs.InstanceDataStepRate = 0u;

	D3D11_INPUT_ELEMENT_DESC layoutDecsList[] =
	{ vertexPosInputDecs ,vertexTexInputDecs };

	int layoutCount = std::size(layoutDecsList);

	ComPtr<ID3D11InputLayout> inputLayout;

	hr = data.device->CreateInputLayout(
		layoutDecsList,
		layoutCount,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	);

	if (hr != S_OK)
	{
		throw std::exception(R"(
		
		Exception : vertex shader doesn't match input alignment or internal error occured. 
		The proper one is :
		struct INPUT {
			float2 pos : POSITION;
			float2 uv : TEXCOORD;
		};
		)");
	}

	if (data.inputLayout == nullptr)
	{
		data.inputLayout = inputLayout;
		data.context->IASetInputLayout(inputLayout.Get());
	}

	data.vertexShaders[index] = shader;
}

void globalState::loadPixelShader(USHORT index, const std::wstring& filePath)
{
	if (data.device == nullptr)
	{
		throw std::exception("\n\n\tException : Attempt to create a shader before graphics is initialized");
	}
	ComPtr<ID3DBlob> blob;
	auto hr = D3DReadFileToBlob(filePath.c_str(), &blob);
	if (hr != S_OK)
	{
		std::cout << "\n\tPixel shader loading failed. Incorrect file path specified or general internal error occured.";
		return;
	}
	ComPtr<ID3D11PixelShader> shader;
	hr = data.device->CreatePixelShader
	(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&shader
	);
	if (hr != S_OK)
	{
		std::cout << "\n\Pixel shader loading failed. Loaded shader isn't a shader or corrupted. Internal error occured.";
		return;
	}
	data.pixelShaders[index] = shader;
}

void globalState::setVertexShader(USHORT index)
{
	if (data.device == nullptr)
	{
		throw std::exception("\n\n\tException : Attempt to set a shader before graphics is initialized");
	}
	if (data.vertexShaders.find(index) == data.vertexShaders.end())
	{
		std::cout << "\n\tWarning. Attempt to set a shader that doesn't exist.";
		return;
	}

	data.context->VSSetShader
	(
		data.vertexShaders[index].Get(),
		NULL,
		NULL
	);
}

void globalState::setPixelShader(USHORT index)
{
	if (data.device == nullptr)
	{
		throw std::exception("\n\n\tException : Attempt to set a shader before graphics is initialized");
	}
	if (data.pixelShaders.find(index) == data.pixelShaders.end())
	{
		std::cout << "\n\tWarning. Attempt to set a shader that doesn't exist.";
		return;
	}

	data.context->PSSetShader
	(
		data.pixelShaders[index].Get(),
		NULL,
		NULL
	);
}

struct float2 {
	float x;
	float y;
};

struct VERTEX_DATA {
	float2 pos;
	float2 uv;
};

void globalState::loadImage(USHORT index, const std::string& filePath)
{
	if (data.device == nullptr)
	{
		throw std::exception("\n\n\tException : Attempt to load an image before graphics is initialized");
	}

	int width, height;
	unsigned char* res = stbi_load(filePath.c_str(), &width, &height, nullptr, 4);
	if (res == nullptr)
	{
		std::cout << "\n\tSpecified image wasn't found";
		return;
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1u;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0u;
	desc.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA textureData = {};
	textureData.pSysMem = (void*)res;
	textureData.SysMemPitch = width * 4u;
	//textureData.SysMemSlicePitch = 0u;

	ComPtr<ID3D11Texture2D> texture;
	auto hr = data.device->CreateTexture2D(&desc, &textureData, &texture);
	if (hr != S_OK) {
		throw std::exception("\n\n\tException : Texture creation failed. Internal error.");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1u;
	viewDesc.Texture2D.MostDetailedMip = 0u;

	ComPtr<ID3D11ShaderResourceView> textureView;
	hr = data.device->CreateShaderResourceView(texture.Get(), &viewDesc, &textureView);
	if (hr != S_OK) {
		throw std::exception("\n\n\tException : Texture view creation failed. Internal error.");
	}
	
	VERTEX_DATA imageRectangle[4] = {};

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = sizeof(imageRectangle);
	bufDesc.StructureByteStride = sizeof(VERTEX_DATA);
	bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;

	{
		auto t_width = width;
		auto t_height = height;
		if (t_width > t_height)
		{
			t_width = data.windowWidth;
			float ratio = (float)width / t_width;
			t_height /= ratio;
		}
		else{
			t_height = data.windowHeight;
			float ratio = (float)height / t_height;
			t_width *= ratio;

		}
	}

	imageRectangle[0] = { {-1.0f,  1.0f}, {0.0f, 0.0f} }; 
	imageRectangle[1] = { { 1.0f,  1.0f}, {1.0f, 0.0f} }; 
	imageRectangle[2] = { {-1.0f, -1.0f}, {0.0f, 1.0f} }; 
	imageRectangle[3] = { { 1.0f, -1.0f}, {1.0f, 1.0f} }; 


	D3D11_SUBRESOURCE_DATA vertexResourceData = {};
	vertexResourceData.pSysMem = imageRectangle;
	//vertexResourceData.SysMemPitch = sizeof(VERTEX_DATA);
	
	ComPtr<ID3D11Buffer> buffer;
	hr = data.device->CreateBuffer(&bufDesc, &vertexResourceData, &buffer);
	if (hr != S_OK)
	{
		throw std::exception("\n\n\tException : vertex buffer creation failed");
	}

	
	imageReference imgRef;
	imgRef.width = width;
	imgRef.height = height;
	imgRef.texture = texture;
	imgRef.resourceView = textureView;
	imgRef.vertexBuf = buffer;
	data.images[index] = imgRef;
}

void globalState::processState()
{
	MSG msg = {};
	while (PeekMessageW(&msg, data.wndPtr, 0u, 0u, PM_REMOVE)) {
		DispatchMessageW(&msg);
	}
	Sleep(1u);
}

void globalState::draw(UINT index)
{
	if (!checkCompleteInit()) return;

	// 1. Get the current frame's backbuffer 
	ComPtr<ID3D11Texture2D> backBuffer;
	data.swapchain->GetBuffer(0u, __uuidof(ID3D11Texture2D), &backBuffer);

	// 2. Create and Bind the RTV for this specific frame 
	data.device->CreateRenderTargetView(backBuffer.Get(), nullptr, &data.target);
	data.context->OMSetRenderTargets(1u, data.target.GetAddressOf(), nullptr);

	// 3. RE-SET VIEWPORT (Essential after OMSetRenderTargets) 
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)data.windowWidth, (float)data.windowHeight, 0.0f, 1.0f };
	data.context->RSSetViewports(1, &vp);

	data.context->ClearRenderTargetView(data.target.Get(), data.clearColor);

	if (data.images.find(index) != data.images.end())
	{
		// 4. Ensure topology and layout are set 
		data.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		data.context->IASetInputLayout(data.inputLayout.Get());

		auto& resRef = data.images[index];
		UINT stride = sizeof(VERTEX_DATA), offset = 0;

		data.context->IASetVertexBuffers(0, 1, resRef.vertexBuf.GetAddressOf(), &stride, &offset);
		data.context->PSSetShaderResources(0, 1, resRef.resourceView.GetAddressOf());

		// 5. Re-bind shaders just in case [cite: 4]
		data.context->VSSetShader(data.vertexShaders[0].Get(), nullptr, 0);
		data.context->PSSetShader(data.pixelShaders[0].Get(), nullptr, 0);

		data.context->Draw(4u, 0u);
	}

	data.swapchain->Present(1u, 0u);
}

void globalState::freeResources()
{
	
	data.vertexShaders.clear();
	data.pixelShaders.clear();
	
	data.device = nullptr;
	data.context = nullptr;
	data.factory = nullptr;
	data.swapchain = nullptr;
	data.query = nullptr;
	data.target = nullptr;
	data.windowWorks = false;
	
	if (data.wndPtr != nullptr)
	{
		DestroyWindow(data.wndPtr);
		UnregisterClassW(data.className,GetModuleHandleW(NULL));
		data.wndPtr = nullptr;
	}

}

bool globalState::checkCompleteInit()
{
	return
		data.device != nullptr &&
		data.wndPtr != nullptr &&
		data.swapchain != nullptr;
}

void globalState::resetState()
{
	freeResources();
	std::cout << "\n\tException thrown. Global state got reset.\n\n";
}

