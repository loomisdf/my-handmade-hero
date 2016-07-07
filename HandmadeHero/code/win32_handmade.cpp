#include <Windows.h>

LRESULT CALLBACK
MainWindowCallback(
	HWND   Window,
	UINT   Message,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message) {

		case WM_SIZE: 
		{
			OutputDebugString("WM_SIZE\n");
			return 0;
		} break;

		case WM_DESTROY: 
		{
			OutputDebugString("WM_DESTROY\n");
		} break;

		case WM_CLOSE: 
		{
			OutputDebugString("VM_CLOSE\n");
		} break;

		case WM_ACTIVATEAPP: 
		{
			OutputDebugString("WM_ACTIVATEAPP\n");
		} break;

		case WM_PAINT: 
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			static DWORD operation = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, operation);
			if (operation == WHITENESS) {
				operation = BLACKNESS;
			}
			else {
				operation = WHITENESS;
			}
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
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	// WindowClass.hIcon = ; // Don't have an icon yet, but we will in the future
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	//MessageBox(0, "This is handmade hero", "Handmade Hero", MB_OK | MB_ICONINFORMATION);

	if (RegisterClass(&WindowClass)) {
		HWND WindowHandle = CreateWindowEx(
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
		if (WindowHandle != NULL) {
			for (;;) {
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0) {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else {
					break; // Break out of for loop
				}
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
