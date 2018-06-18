#include "../includes.h"

namespace UTILS
{
	namespace INPUT
	{
		long old_wnd_proc;

		void InputHandler::Init()
		{
			for (int& key : cached_pressed_keys)
				key = 0;

			old_wnd_proc = SetWindowLongPtr(GLOBAL::csgo_hwnd, GWL_WNDPROC, (long)WndProcCallback);
		}

		void InputHandler::Update()
		{
			cached_keys_pressed.clear();
			for (int& key : cached_pressed_keys)
				(key > 0) ? key-- : key = 0; // decrement key press count until reaches 0
		}

		MouseInfo InputHandler::GetMouseInfo()
		{
			cached_mouse_info.position = GetMousePosition();

			int right = GetAsyncKeyState(VK_RBUTTON);
			int left = GetAsyncKeyState(VK_LBUTTON);

			cached_mouse_info.left = left;
			cached_mouse_info.right = right;

			auto ret = cached_mouse_info;

			cached_mouse_info.scroll = 0;
			return ret;
		}

		void InputHandler::GetCurrentKeyboardState(int* keyboard)
		{
			memcpy(keyboard, cached_pressed_keys, sizeof(int) * 256);
		}

		void InputHandler::GetKeysPressed(std::vector<int>& keys_pressed)
		{
			keys_pressed = cached_keys_pressed;
		}

		int InputHandler::GetKeyState(int key)
		{
			return cached_pressed_keys[key];
		}

		int InputHandler::GetKeyStateEx(int key)
		{
			int ret = cached_pressed_keys_ex[key];

			cached_pressed_keys_ex[key] = max(0, cached_pressed_keys_ex[key] - 1);

			return ret;
		}

		void InputHandler::Callback(UINT Message, WPARAM wParam, LPARAM lParam)
		{
			switch (Message)
			{
				case WM_KEYDOWN:
					if (wParam >= 0 && wParam < 256)
					{
						cached_pressed_keys_ex[wParam]++;
						cached_pressed_keys[wParam]++;
						cached_keys_pressed.push_back(wParam);
					}
					break;
				case WM_XBUTTONDOWN:

					if (GET_XBUTTON_WPARAM(wParam) & XBUTTON1)
					{
						cached_pressed_keys[VK_XBUTTON1]++;
						cached_pressed_keys_ex[VK_XBUTTON1]++;
						cached_keys_pressed.push_back(VK_XBUTTON1);
					}
					else if (GET_XBUTTON_WPARAM(wParam) & XBUTTON2)
					{
						cached_pressed_keys[VK_XBUTTON2]++;
						cached_pressed_keys_ex[VK_XBUTTON2]++;
						cached_keys_pressed.push_back(VK_XBUTTON2);
					}

					break;
				case WM_MOUSEWHEEL:
					cached_mouse_info.scroll = GET_WHEEL_DELTA_WPARAM(wParam);
					break;
				case WM_MBUTTONDOWN:
					cached_pressed_keys[VK_MBUTTON]++;
					cached_pressed_keys_ex[VK_MBUTTON]++;
					cached_keys_pressed.push_back(VK_MBUTTON);
					break;
			}
		}

		char InputHandler::KeyToChar(int key)
		{
			BYTE kb[256];
			GetKeyboardState(kb);
			WCHAR buffer[5];

			if (ToUnicode(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), kb, buffer, 1, 0) == 1)
			{
				_bstr_t b(buffer);
				char* c = b;
				return c[0];
			}
			else
				return 0;
		}

		Vector2D InputHandler::GetMousePosition()
		{
			POINT mouse_position;
			GetCursorPos(&mouse_position);
			ScreenToClient(GLOBAL::csgo_hwnd, &mouse_position);
			return {static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y)};
		}

		LRESULT CALLBACK WndProcCallback(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
		{
			input_handler.Callback(Message, wParam, lParam);
			return CallWindowProc((WNDPROC)old_wnd_proc, Hwnd, Message, wParam, lParam);
		}

		InputHandler input_handler;
	}
}
