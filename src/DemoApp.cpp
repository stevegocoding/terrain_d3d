#include "DemoApp.h"
#include "DDreamMap.h"

// global instance handler
static HINSTANCE g_hInstance = 0;
static CDDMap g_mapWndDC;

BEGIN_MAP_MESSAGE(CDDApplication,CDDWindow)
	MAP_MESSAGE(WM_DESTROY,CDDApplication::OnDestory)
END_MAP_MESSAGE()


const WMessageFunc CDDWindow::messageEntries[] = 
{
	{0,0},
	{0,0}
};

CDDWindow::CDDWindow() : m_wndHandle(0)
{
	int i = 1;
}

CDDWindow::~CDDWindow()
{
}

// windows窗口消息处理函数
LRESULT CALLBACK CDDWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 
	CDDWindow *wndPtr = (CDDWindow*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if (wndPtr) {
		const WMessageFunc *mf = wndPtr->GetMessageEntries();
		do {
			int i;
			for (i=1;mf[i].func;i++) {
				if (mf[i].id==message) {
					// if the function call is failed 
					if ((wndPtr->*(mf[i].func))((DWORD)wParam,(DWORD)lParam) == -1)
						return 0;
					else break;
				}
			}
			mf=(const WMessageFunc*)(mf[0].id);
		}while(mf != 0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CDDWindow::InitWindow(HINSTANCE hInstance)
{
	WNDCLASS	wc;
	if (g_hInstance!=0) return;	// 已经初始化过
	g_hInstance = hInstance;
	wc.style=          CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=    CDDWindow::WndProc;
	wc.cbClsExtra=     0;
	wc.cbWndExtra=     0;
	wc.hInstance=      g_hInstance;
	wc.hIcon=          LoadIcon(hInstance,"WS_ICON");
	wc.hCursor=        NULL;
	wc.hbrBackground=  (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName=   NULL;
	wc.lpszClassName=  "DiabloDreamWindow";
    RegisterClass(&wc);
}

bool CDDWindow::createWindow(const char *name,
					DWORD style,
					int width, int height, int xPos, int yPos,
					HWND parent)
{
	if (m_wndHandle != 0)
		return false;

	m_wndHandle = ::CreateWindow("DiabloDreamWindow",name,style,xPos,yPos,width,height,parent,0,g_hInstance,0);
	if (m_wndHandle == 0)
		return false;

	// put THIS pointer into the extra memory of this window
	::SetWindowLongPtr(m_wndHandle,GWLP_USERDATA,(DWORD)this);
	return true;
}

bool CDDWindow::createWindowEx(DWORD exstyle, 
					const char *name,
					DWORD style,
					int width, int height, int xPos, int yPos,
					HWND parent)
{
	if (m_wndHandle != 0)
		return false;

	m_wndHandle = ::CreateWindowEx(exstyle,"DiabloDreamWindow",name,style,xPos,yPos,width,height,parent,0,g_hInstance,0);
	if (m_wndHandle == 0)
		return false;
	
	// put THIS pointer into the extra memory of this window
	::SetWindowLongPtr(m_wndHandle,GWLP_USERDATA,(DWORD)this);
	return true;
}

void CDDWindow::setText(char *title)
{
	::SetWindowText(m_wndHandle,title);
}

char *CDDWindow::getText() const
{
	static char buf[256];
	::GetWindowText(m_wndHandle,buf,256);
	return buf;
}

int CDDWindow::show(int show) const
{
	return (::ShowWindow(m_wndHandle,show));
}

int CDDWindow::update() const
{
	return (::UpdateWindow(m_wndHandle));
}

HDC CDDWindow::getDC() const
{
	HDC LastDC=(HDC)g_mapWndDC.lookUp((void*)m_wndHandle);
	if (LastDC==0) {
		LastDC=::GetDC(m_wndHandle);
		g_mapWndDC.insert((void*)m_wndHandle,(void *)LastDC);
	}
	return LastDC;
}

void CDDWindow::releaseDC(HDC hDC)
{
	g_mapWndDC.del((void*)m_wndHandle);
	::ReleaseDC(m_wndHandle,hDC);
}

LPRECT CDDWindow::getClientRect()
{
	static RECT clientRect;
	::GetClientRect(m_wndHandle,&clientRect);
	return &clientRect;
}

////////////////////////////////////////////////////////////////////


CDDApplication::CDDApplication(HINSTANCE hInstance)
{
	// Init the window,register the window class
	CDDWindow::InitWindow(hInstance);
}

CDDApplication::~CDDApplication()
{
}

// Exit the application
int CDDApplication::OnDestory(DWORD wParam,DWORD lParam)
{
	quit();
	return 0;
}

bool CDDApplication::createMainWindow()
{
	// Create a main game window
	return createWindow("DiabloDream");
}

void CDDApplication::run()
{
	// There is the game loop
	MSG msg;
	afterCreate();
	show();
	update();
	m_timer.Init();
	static int frameCount = 0;
	for (;;)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
        {
            if(!GetMessage(&msg,NULL,0,0))
				break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else		
		{
			idle();
			frameCount ++;
			if (frameCount > 200) {
				m_fps = m_timer.GetFPS() * 200;
				frameCount = 0;
			}
		}
    }	
}

void CDDApplication::quit()
{
	beforeExit();
	PostQuitMessage(0);
}

