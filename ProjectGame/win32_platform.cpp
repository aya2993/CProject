#include "utils.cpp"
#include <Windows.h>

global_variable bool running = true;

struct RenderState{
	int height, width;
	void* memory;

	BITMAPINFO BitmapInfo;
};

global_variable RenderState renderState;

#include "render.cpp"
#include "platformCommon.cpp"
#include "game.cpp"

LRESULT CALLBACK windowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// handle to window,  message identifier,  first message parameter, second message parameter
{
	LRESULT result = 0;
	switch (uMsg){
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
	} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			renderState.width = rect.right - rect.left;
			renderState.height = rect.bottom - rect.top;

			int size = renderState.width * renderState.height * sizeof(unsigned int);

			if (renderState.memory) VirtualFree(renderState.memory, 0, MEM_RELEASE);
			renderState.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			renderState.BitmapInfo.bmiHeader.biSize = sizeof(renderState.BitmapInfo.bmiHeader);
			renderState.BitmapInfo.bmiHeader.biWidth = renderState.width;
			renderState.BitmapInfo.bmiHeader.biHeight = renderState.height;
			renderState.BitmapInfo.bmiHeader.biPlanes = 1;
			renderState.BitmapInfo.bmiHeader.biBitCount = 32;
			renderState.BitmapInfo.bmiHeader.biCompression = BI_RGB;
		}break;

		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
	
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	//window class
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = "Game Window Class";
	windowClass.lpfnWndProc = windowCallback;

	//register class
	RegisterClass(&windowClass);

	//create window
	HWND window = CreateWindow(windowClass.lpszClassName, "Game Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	HDC HDC = GetDC(window);

	Input input = {};

	float deltaTime = 0.016666f;
	LARGE_INTEGER frameBeginTime;
	QueryPerformanceCounter(&frameBeginTime);

	float performanceFrequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performanceFrequency = (float)perf.QuadPart;
	}

	while (running) {
		//Input 
		MSG message;

		for (int i = 0; i < Button_Count; i++) {
			input.buttons[i].changed = false;
		}

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {

			switch (message.message) {
			case WM_KEYUP:
			case WM_KEYDOWN: {
				u32 vkCode = (u32)message.wParam;
				bool isDown = ((message.lParam & (1 << 31)) == 0);

#define process_button(b, vk) \
case vk: {\
input.buttons[b].isDown = isDown;\
input.buttons[b].changed = true;\
}break;\

				switch (vkCode) {
					process_button(Button_Up, VK_UP);
					process_button(Button_Down, VK_DOWN);
					process_button(Button_Left, VK_LEFT);
					process_button(Button_Right, VK_RIGHT);

					process_button(Button_W, 'W');
					process_button(Button_S, 'S');
				}
			}break;


			default: {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			}

		}
		//Simulate
		simulateGame(&input, deltaTime);


		//Render
		StretchDIBits(HDC, 0, 0, renderState.width, renderState.height, 0, 0, renderState.width, renderState.height, renderState.memory, &renderState.BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frameEndTime;
		QueryPerformanceCounter(&frameEndTime);
		deltaTime = (float)(frameEndTime.QuadPart - frameBeginTime.QuadPart) / performanceFrequency;
		frameBeginTime = frameEndTime;
	}
}

	