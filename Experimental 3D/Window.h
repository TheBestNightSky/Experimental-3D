#pragma once
#include "SkyeWin.h"
#include "SkyenetException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>
#include "WndInfo.h"


class Window {
public:
	class Exception : public SkyenetException {

	public:

		Exception(int line, const char* file, HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;

	};

private:
	class WindowClass {
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"SkyeNet3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::wstring& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() noexcept;
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();

private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT WINAPI MsgHandlerSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	static LRESULT WINAPI MsgHandlerHandoff( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	LRESULT MsgHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;

public:
	Keyboard kbd;
	Mouse mouse;
	WndInfo GetWndInfo();
private:
	bool cursorEnabled = true;
	int width;
	int height;
	HWND hWnd;
	WndInfo winfo;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};

// exception helper macro
#define SKWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define SKWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError() )