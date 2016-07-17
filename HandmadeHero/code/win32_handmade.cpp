#include <Windows.h>
#include <stdint.h>
#include "vector2D.h"


#define internal static 
#define local_persist static 
#define global_variable static 

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t	 uint32;
typedef uint64_t	 uint64;

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo; // Note (Daltin): All members are initialized to zero
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
	vector2D v1(5, 5);
	vector2D v2(5, 5);
	vector2D v3 = -1 * v1;

	int Width = BitmapWidth;
	int Height = BitmapHeight;

	int Pitch = Width * BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{
			uint8 PAD = 0;
			uint8 RED = 0;
			uint8 BLUE = (X + XOffset);
			uint8 GREEN = (Y + YOffset);

			/*
				Memory:		BB GG RR xx
				Register:		xx RR GG BB

				Pixel (32-bits)
			*/

			*Pixel++ = ((GREEN << 8) | BLUE);
		}
		Row += Pitch;
	}

}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
	if (BitmapMemory) 
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	// How many bits do we need?
	// We asked for 32 previously ^^^^^
	int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = ClientRect->right - ClientRect->left;
	int WindowHeight = ClientRect->bottom - ClientRect->top;
	StretchDIBits(DeviceContext, 
					  /*
					  X, Y, Width, Height,
					  X, Y, Width, Height,
					  */
					  0, 0, BitmapWidth, BitmapHeight,
					  0, 0, WindowWidth, WindowHeight,
					  BitmapMemory,
					  &BitmapInfo,
					  DIB_RGB_COLORS,
					  SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND   Window,
								UINT   Message,
								WPARAM WParam,
								LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Height = ClientRect.bottom - ClientRect.top;
			int Width = ClientRect.right - ClientRect.left;
			Win32ResizeDIBSection(Width, Height);
			OutputDebugString("WM_SIZE\n");
			return 0;
		} break;

		case WM_CLOSE:
		{
			Running = false;
			OutputDebugString("VM_CLOSE\n");
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugString("WM_ACTIVATEAPP\n");
		} break;

		case WM_DESTROY:
		{
			Running = false;
			OutputDebugString("WM_DESTROY\n");
		} break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;static 
			int Y = Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			RECT ClientRect;
			GetClientRect(Window, &ClientRect);

			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &Paint);
		} break;

		default: {
			// OutputDebugString("default\n");
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}
	return(Result);
}


int CALLBACK WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR     CommandLine,
	int       ShowCode)
{

	WNDCLASS WindowClass = {}; // Note (Daltin): empty curly brackets initializes all values to 0

	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// WindowClass.hIcon = ; // Don't have an icon yet, but we will in the future
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	//MessageBox(0, "This is handmade hero", "Handmade Hero", MB_OK | MB_ICONINFORMATION);

	if (RegisterClassA(&WindowClass)) {
		HWND Window = CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0);
		DWORD error = GetLastError();
		if (Window != NULL) 
		{
			int XOffset = 0;
			int YOffset = 0;

			Running = true;
			while (Running) 
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				RenderWeirdGradient(XOffset, YOffset);
				HDC DeviceContext = GetDC(Window);

				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
				ReleaseDC(Window, DeviceContext);
				++XOffset;
			}
		}
		else {
			// TODO Implement Logging
		}
	}
	else {
		// TODO Implement Logging
	}

	return(0);
}
