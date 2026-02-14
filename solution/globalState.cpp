

#include "globalState.h"

globalState::state_data globalState::data;

void globalState::initGraphics()
{
	auto hr = D3D11CreateDevice
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED |
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
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

	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

	ComPtr<ID3D11Texture2D> backBuffer;
	hr = data.swapchain->GetBuffer(0u, __uuidof(ID3D11Texture2D), &backBuffer);
	
	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nDrawing(2) surface creation failed\n\n");
	}

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc = {};
	targetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Texture2D.MipSlice = 0u;
	hr = data.device->CreateRenderTargetView
	(
		backBuffer.Get(), 
		&targetDesc,
		&data.target
	);

	if (hr != S_OK)
	{
		resetState();
		throw std::exception("\n\nDrawing(3) surface creation failed\n\n");
	}

	data.context->OMSetRenderTargets
	(
		0u, 
		data.target.GetAddressOf(),
		nullptr
	);


}

bool globalState::windowIsOpen()
{
	return data.windowWorks;
}

void globalState::processState()
{
	MSG msg = {};
	while (PeekMessageW(&msg, data.wndPtr, 0u, 0u, PM_REMOVE)) {
		DispatchMessageW(&msg);
	}
}

void globalState::draw()
{
	if (!checkCompleteInit()) {
		throw std::exception("\n\nDrawing cannot be performed till all the things are initialized\n\n");
	}
	
	data.context->Begin(data.query.Get());
	data.context->ClearRenderTargetView(data.target.Get(), data.clearColor);
	data.context->End(data.query.Get());

	data.swapchain->Present(1u,0u);
}

void globalState::freeResources()
{
	data.device = nullptr;
	data.context = nullptr;
	data.factory = nullptr;
	data.swapchain = nullptr;
	
	for (auto& shader : data.vertexShaders)
	{
		shader.second = nullptr;
	}
	data.vertexShaders.clear();

	for (auto& shader : data.pixelShaders)
	{
		shader.second = nullptr;
	}
	data.pixelShaders.clear();

	if (data.wndPtr != nullptr)
	{
		DestroyWindow(data.wndPtr);
		UnregisterClassW(data.className,GetModuleHandleW(NULL));
	}

}

bool globalState::checkCompleteInit()
{
	return
		data.device != nullptr &&
		data.wndPtr != nullptr;
}

void globalState::resetState()
{
	std::cout << "\n\tException thrown. Global state got reset.\n\n";
}
