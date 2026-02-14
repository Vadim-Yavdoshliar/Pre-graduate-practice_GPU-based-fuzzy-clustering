

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
		D3D11_CREATE_DEVICE_DEBUG |
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL,
		0u,
		D3D11_SDK_VERSION,
		&data.device,
		NULL,
		&data.context
	);

	if(hr != S_OK)
	throw std::exception("\n\nBasic graphics resources creation failed.\n\n");

}


LRESULT globalState::windowProcedure(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

}


void globalState::initWindow(USHORT width, USHORT height)
{
	auto className = L"GRAPHICS_WINDOW";

	WNDCLASSW desc = {};
	desc.lpfnWndProc = globalState::windowProcedure;
	desc.lpszClassName = className;
	desc.hCursor = NULL;
	desc.hInstance = GetModuleHandleW(NULL);
	desc.cbClsExtra = NULL;
	desc.cbWndExtra = NULL;
	desc.hbrBackground = NULL;
	desc.hIcon = NULL;
	desc.lpszMenuName = NULL;
	desc.style = NULL;
	RegisterClass(&desc);

	// GetSystemMetrics()

	CreateWindowExW
	(
		NULL,
		className,
		L"Output window",
		WS_CAPTION |
		WS_MAXIMIZEBOX | 
		WS_MINIMIZEBOX |
		WS_SIZEBOX,

	)
}
	