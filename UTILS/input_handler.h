#pragma once

namespace UTILS
{
	namespace INPUT
	{
		LRESULT CALLBACK WndProcCallback(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

		struct MouseInfo
		{
			Vector2D position;

			int scroll;
			int left;
			int right;
		};

		class InputHandler
		{
		public:
			void Init();
			void Update();

			MouseInfo GetMouseInfo();

			void GetCurrentKeyboardState(int*);
			void GetKeysPressed(std::vector<int>& keys_pressed);
			int GetKeyState(int);
			int GetKeyStateEx(int key);

			char KeyToChar(int key);

			void Callback(UINT, WPARAM, LPARAM);

		private:
			MouseInfo cached_mouse_info;
			std::vector<int> cached_keys_pressed;
			int cached_pressed_keys[256];
			int cached_pressed_keys_ex[256]; // these are only reset when you call GetKeyStateEx()

			Vector2D GetMousePosition();
		};

		extern InputHandler input_handler;
	}
}
