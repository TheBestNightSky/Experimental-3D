#include "Window.h"
#include "SkyenetException.h"
#include "SkyeFun.h"
#include <sstream>
#include <string>
#include "resource.h"
#include "imgui/imgui_impl_win32.h"
// =============================================== //
// ======== WindowClass class definitions ======== //
// =============================================== //

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)){

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = MsgHandlerSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 48,48,0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0));

	RegisterClassEx(&wc);

}
		
//destructor unregisters the winapi window class
Window::WindowClass::~WindowClass() {

	UnregisterClass(wndClassName, GetInstance());

}

const wchar_t* Window::WindowClass::GetName() noexcept {

	return wndClassName;

}

HINSTANCE Window::WindowClass::GetInstance() noexcept {

	return wndClass.hInst;

}

// ========================================== //
// ======== Window class definitions ======== //
// ========================================== //

Window::Window(int width, int height, const wchar_t* name) : width(width), height(height){

	// calculate size of window based on client region size
	RECT rect;
	rect.left = 100;
	rect.right = width + rect.left;
	rect.top = 100;
	rect.bottom = height + rect.top;

	if (AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0) {

		throw SKWND_LAST_EXCEPT();
	}

	// create window and get hWnd
	hWnd = CreateWindow( WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, WindowClass::GetInstance(), this );
	if (hWnd == nullptr) {
		throw SKWND_LAST_EXCEPT();
	}
	// show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );

	//init imgui win32 impl
	ImGui_ImplWin32_Init(hWnd);

	//creat wndinfo
	winfo = WndInfo(hWnd);
	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd, winfo);
}

WndInfo Window::GetWndInfo(){
	return winfo;
}

Window::~Window() {
	ImGui_ImplWin32_Shutdown();
	DestroyWindow( hWnd );
}

void Window::SetTitle(const std::wstring& title) {
	if (SetWindowText(hWnd, title.c_str()) == 0) {

		throw SKWND_LAST_EXCEPT();
	}
}

Graphics& Window::Gfx() {

	return *pGfx;
}

std::optional<int> Window::ProcessMessages() {
	MSG msg;
	// while message queue is not empty remove and dispatch messages
	// does not block if message queue is empty
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		// check for quit message because peekmessage will not return an exit code
		if (msg.message == WM_QUIT) {
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// return empty optional if not quiting
	return {};
}

LRESULT WINAPI Window::MsgHandlerSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept {

	if (msg == WM_NCCREATE) {

		// get pointer to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		Window* const pWnd = static_cast<Window*>( pCreate->lpCreateParams );
		
		// set WinAPI-managed user data to store pointer to window class
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
		
		// set message proc to normal handler now that setup is complete
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( &MsgHandlerHandoff ) );
		
		// forward message to window class handler
		return pWnd->MsgHandler(hWnd, msg, wParam, lParam);

	}

	// if a msg is recieved before WM_NCCREATE, handle with the default proc
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT WINAPI Window::MsgHandlerHandoff(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {

	// retrieve pointer to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->MsgHandler(hWnd, msg, wParam, lParam);

}
	
LRESULT Window::MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	// clear all keystates when the window loses focus
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

	// Keyboard event messages //
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (imio.WantCaptureKeyboard) {

			break;
		}

		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) {

			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (imio.WantCaptureKeyboard) {

			break;
		}

		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:

		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;

	// END OF KEYBOARD EVENT MESSAGES

	// Mouse event messages //
	case WM_MOUSEMOVE: {
		if (imio.WantCaptureMouse) {

			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);

		// if mouse is in client region trigger Move event
		// if mouse moves into client region trigger Enter event and capture mouse
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {

			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow()) {

				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// if button down trigger Move event and maintain capture
		// else if not in client region release capture
		else {

			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) {

				mouse.OnMouseMove(pt.x, pt.y);
			}
			else {

				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}

	case WM_LBUTTONDOWN: {
		SetForegroundWindow(hWnd);
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}

	case WM_MBUTTONDOWN: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMiddlePressed(pt.x, pt.y);
		break;
	}

	case WM_RBUTTONDOWN: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}

	case WM_LBUTTONUP: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);

		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height) {
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}

	case WM_MBUTTONUP: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMiddleReleased(pt.x, pt.y);

		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height) {
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}

	case WM_RBUTTONUP: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMiddleReleased(pt.x, pt.y);

		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height) {
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}

	case WM_MOUSEWHEEL: {
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}

	// END OF MOUSE EVENT MESSAGES //

	// Raw mouse event messages //

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}


// ==================================================== //
// ======== Window Exception class definitions ======== //
// ==================================================== //

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept : SkyenetException(line, file), hr(hr){}

const char* Window::Exception::what() const noexcept {

	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* Window::Exception::GetType() const noexcept {

	return "Skyenet Window Exception";

}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept {
	
	WCHAR* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, 0,
		reinterpret_cast<LPWSTR>(&pMsgBuf),0,nullptr
		);

	if (nMsgLen == 0) {
		return "Unidentified error code";
	}

	std::wstring wstr = pMsgBuf;
	std::string errorString(wstr.begin(), wstr.end());
	LocalFree(pMsgBuf);
	return errorString;

}

HRESULT Window::Exception::GetErrorCode() const noexcept {

	return hr;

}

std::string Window::Exception::GetErrorString() const noexcept {
	
	return TranslateErrorCode(hr);

}