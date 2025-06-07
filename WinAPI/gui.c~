#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	    if (uMsg == WM_DESTROY) {
		            PostQuitMessage(0);
			            return 0;
				        }
	        return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		                   LPSTR lpCmdLine, int nCmdShow) {
	    const char CLASS_NAME[] = "MyWindowClass";

	        WNDCLASS wc = {0};
		    wc.lpfnWndProc = WindowProc;
		        wc.hInstance = hInstance;
			    wc.lpszClassName = CLASS_NAME;
			        RegisterClass(&wc);

				    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "My GUI App",
						            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
							            NULL, NULL, hInstance, NULL);

				        if (hwnd == NULL) return 0;

					    ShowWindow(hwnd, nCmdShow);

					        MSG msg = {0};
						    while (GetMessage(&msg, NULL, 0, 0)) {
							            TranslateMessage(&msg);
								            DispatchMessage(&msg);
									        }

						        return 0;
}

