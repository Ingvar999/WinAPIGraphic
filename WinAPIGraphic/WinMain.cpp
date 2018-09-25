#include <windows.h>
#include <math.h>

const UINT W_KEY = 0x57;
const UINT A_KEY = 0x41;
const UINT S_KEY = 0x53;
const UINT D_KEY = 0x44;
const UINT MENU_ID = 33;
const int width = 80;
const int height = 80;
const int speed = 5;
const float rotatespeed = 0.1;
const float formspeed = 0.05;

TCHAR file[_MAX_PATH];
const TCHAR *filter = "*.bmp\0\0";

float angle = 0;
float xForm, yForm = 0;
HBITMAP image = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam);
void DrawPicture(HWND hWnd, INT16 x, INT16 y, bool delta = false);
void KeyDownHandler(HWND hWnd, WPARAM key, LPARAM params);
void MouseWheelHandler(HWND hWnd, WPARAM params, LPARAM position);
void Rotate(float radians, INT16 x, INT16 y, HWND hWnd, HBITMAP hBitmap);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "ObjectMovingClass";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow("ObjectMovingClass", "Lab #1 OSiSP",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HDC dc = GetDC(hWnd);
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(0, 0, 255);
	HBRUSH brush = CreateBrushIndirect(&logBrush);
	SelectObject(dc, brush);
	image = CreateCompatibleBitmap(dc, width, height);
	SelectObject(dc, image);
	Rectangle(dc, 0, 0, width, height);


	HMENU menu = CreateMenu();
	AppendMenu(menu, 0, MENU_ID, "Open Picture");
	SetMenu(hWnd, menu);

	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message){
		case WM_MOUSEMOVE:
			DrawPicture(hWnd, lParam, lParam >> 16);
			break;
		case WM_KEYDOWN:
			KeyDownHandler(hWnd, wParam, lParam);
			break;
		case WM_MOUSEWHEEL:
			MouseWheelHandler(hWnd, wParam, lParam);
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == MENU_ID) {
				OPENFILENAME openFile;
				ZeroMemory(&openFile, sizeof(openFile));

				openFile.lStructSize = sizeof(openFile);
				openFile.hwndOwner = hWnd;
				openFile.lpstrFile = (LPSTR)file;
				openFile.nMaxFile = sizeof(file);
				openFile.lpstrFilter = filter;
				openFile.nFilterIndex = 1;
				openFile.lpstrFileTitle = NULL;
				openFile.nMaxFileTitle = NULL;
				openFile.lpstrInitialDir = NULL;
				openFile.lpstrTitle = "Open...";
				openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				openFile.hInstance = 0;
				if (GetOpenFileNameA(&openFile)) {
					image = (HBITMAP)LoadImageA(NULL, openFile.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			exit(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void KeyDownHandler(HWND hWnd, WPARAM key, LPARAM params) {
	switch (key) {
	case VK_LEFT:
		angle -= rotatespeed;
		DrawPicture(hWnd, 0, 0, true);
		break;
	case VK_RIGHT:
		angle += rotatespeed;
		DrawPicture(hWnd, 0, 0, true);
		break;
	case VK_UP:
		yForm -= formspeed;
		DrawPicture(hWnd, 0, 0, true);
		break;
	case VK_DOWN:
		yForm += formspeed;
		DrawPicture(hWnd, 0, 0, true);
		break;
	case W_KEY:
		DrawPicture(hWnd, 0, -speed, true);
		break;
	case S_KEY:
		DrawPicture(hWnd, 0, speed, true);
		break;
	case A_KEY:
		DrawPicture(hWnd, -speed, 0, true);
		break;
	case D_KEY:
		DrawPicture(hWnd, speed, 0, true);
		break;
	case VK_ESCAPE:
		if (MessageBox(hWnd, "Do you want to exit?", "Exit", MB_YESNO) == IDYES)
			PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	}
}

void MouseWheelHandler(HWND hWnd, WPARAM params, LPARAM position) {
	int wheelDelta = 0;
	wheelDelta += GET_WHEEL_DELTA_WPARAM(params);
	if (params & MK_SHIFT)
		DrawPicture(hWnd, -wheelDelta, 0, true);
	else
		DrawPicture(hWnd, 0, wheelDelta, true);
}

void DrawPicture(HWND hWnd, INT16 x, INT16 y, bool delta) {
	static int oldx = 0, oldy = 0;
	if (delta) {
		RECT border;
		GetClientRect(hWnd, &border);
		x = oldx + x;
		y = oldy + y;
		if (x < 0 || y < 0 || (x + width) >(border.right - border.left) || (y + height) >(border.bottom - border.top)) {
			x = oldx;
			y = oldy;
		}
	}
	Rotate(angle, x, y, hWnd, image);
	oldx = x;
	oldy = y;
}

void Rotate(float radians, INT16 x, INT16 y, HWND hWnd, HBITMAP hBitmap)
{
	HDC sourceDC, destDC;
	destDC = GetDC(hWnd);
	sourceDC = CreateCompatibleDC(destDC);

	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	HBITMAP hbmOldSource = (HBITMAP)SelectObject(sourceDC, hBitmap);

	SetGraphicsMode(destDC, GM_ADVANCED);

	float m1[3][3] = {
		{cosine, sine, 0},
		{-sine, cosine, 0},
		{x, y, 1}
	};

	float m2[3][3] = {
		{1, yForm, 0},
		{xForm, 1, 0},
		{0, 0, 1}
	};

	float m3[3][3] = { 0 };

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				m3[i][j] += m1[i][k] * m2[k][j];

	XFORM xform;
	xform.eM11 = m3[0][0];
	xform.eM12 = m3[0][1];
	xform.eM21 = m3[1][0];
	xform.eM22 = m3[1][1];
	xform.eDx = m3[2][0];
	xform.eDy = m3[2][1];

	SetWorldTransform(destDC, &xform);

	RECT clntRect;
	GetClientRect(hWnd, &clntRect);
	InvalidateRect(hWnd, &clntRect, true);
	UpdateWindow(hWnd);

	StretchBlt(destDC, 0, 0, width, height, sourceDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	//TransparentBlt(destDC, 0, 0, width, height, sourceDC, 0, 0, w, h, RGB(255,0,255));
	
	SelectObject(sourceDC, hbmOldSource);
	DeleteDC(sourceDC);
	ReleaseDC(hWnd, destDC);
}

