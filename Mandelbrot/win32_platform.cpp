#include <windows.h>
#include <windowsx.h>
#include "mandelbrot.h"
#include <thread>
#include <iostream>

bool running = true;

void* buffer_memory;
int buffer_width;
int buffer_height;
BITMAPINFO buffer_bitmap_info;

double x_center = -0.765;
double y_center = 0;
double scale_x;
double scale_y;


void fillBuffer() {
	unsigned int* pixel = (unsigned int*)buffer_memory;
	std::vector<std::thread> ThreadVector;

	for (int y = 0; y < buffer_height; y++) {


		ThreadVector.emplace_back([=]() {calcMandelRow(y, buffer_width, buffer_height, pixel, x_center, y_center, scale_x, scale_y); });

		pixel += buffer_width;

	}

	for (auto& t : ThreadVector) {
		t.join();
	}
}

double mouseXToWorldX(double mouse_x) {
	return x_center + scale_x * (mouse_x - buffer_width / 2);
}

double mouseYToWorldY(double mouse_y) {
	return y_center + scale_y * (buffer_height / 2 - mouse_y);
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
	} break;

	case WM_LBUTTONDOWN: {

		double mouse_x = GET_X_LPARAM(lParam);
		double mouse_y = GET_Y_LPARAM(lParam);

		x_center += (mouseXToWorldX(mouse_x) - x_center) / 2;
		y_center += (mouseYToWorldY(mouse_y) - y_center) / 2;

		scale_x *= 0.5;
		scale_y *= 0.5;

		fillBuffer();

	};

	case WM_RBUTTONDOWN: {


		double mouse_x = GET_X_LPARAM(lParam);
		double mouse_y = GET_Y_LPARAM(lParam);

		x_center += (mouseXToWorldX(mouse_x) - x_center) / 2;
		y_center += (mouseYToWorldY(mouse_y) - y_center) / 2;

		scale_x *= 1.1;
		scale_y *= 1.1;

		fillBuffer();

	};

	case WM_KEYDOWN:
		switch (wParam) {

		case VK_LEFT: {
			x_center -= buffer_width * 0.1 * scale_x;
			fillBuffer();
		} break;

		case VK_RIGHT: {
			x_center += buffer_width * 0.1 * scale_x;
			fillBuffer();
		} break;

		case VK_UP: {
			y_center += buffer_height * 0.1 * scale_y;
			fillBuffer();
		} break;

		case VK_DOWN: {
			y_center -= buffer_height * 0.1 * scale_y;
			fillBuffer();
		} break;
		} break;

	case WM_MOUSEWHEEL: {
		int zDelta = - GET_WHEEL_DELTA_WPARAM(wParam) / 100;


		if (zDelta < 0) {
			scale_x /= -zDelta;
			scale_y /= -zDelta;
		}

		else {
			scale_x *= zDelta;
			scale_y *= zDelta;
		}


		

		fillBuffer();

	} break;

	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		buffer_width = rect.right - rect.left;
		buffer_height = rect.bottom - rect.top;
		scale_x = 2.47f / buffer_width;
		scale_y = 2.24f / buffer_height;

		int buffer_size = buffer_width * buffer_height * sizeof(unsigned int);

		if (buffer_memory) VirtualFree(buffer_memory, 0, MEM_RELEASE);

		buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
		buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
		buffer_bitmap_info.bmiHeader.biHeight = buffer_height;
		buffer_bitmap_info.bmiHeader.biPlanes = 1;
		buffer_bitmap_info.bmiHeader.biBitCount = 32;
		buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

		fillBuffer();


	}

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}

	return result;
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Madelbrot";
	window_class.lpfnWndProc = window_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindowEx(0, window_class.lpszClassName, L"Mandelbrot", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	HDC hdc = GetDC(window);


	if (window == NULL)
	{
		return 0;
	}

	ShowWindow(window, nShowCmd);

	while (running){

		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)){
			TranslateMessage(&message);
			DispatchMessage(&message);
			
		}
		
		StretchDIBits(hdc, 0, 0, buffer_width, buffer_height, 0, 0, buffer_width, buffer_height, buffer_memory, &buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);

	}

	return 0;

}