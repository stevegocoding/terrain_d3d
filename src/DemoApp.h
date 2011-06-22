#ifndef _DDAPPLICATION_H_
#	define _DDAPPLICATION_H_

#if _MSC_VER > 1000
#	pragma once
#endif 

#include "prerequisites.h"
#include "timer.h"

class CDDWindow;

typedef int (CDDWindow::*WndMsgHdler)(DWORD,DWORD);

// Message handling function
struct WMessageFunc {
	DWORD id;
	WndMsgHdler func;
};

#define DECLARE_MAP_MESSAGE()								\
public:													\
virtual const WMessageFunc* GetMessageEntries() { return messageEntries; }	\
static const WMessageFunc messageEntries[];

#define BEGIN_MAP_MESSAGE(thisclass,baseclass)			\
const WMessageFunc thisclass::messageEntries[]={		\
	{(DWORD)(baseclass::messageEntries),0},

#define MAP_MESSAGE(id,func) {id,(WndMsgHdler)&func},
#define END_MAP_MESSAGE() {0,0}};


class CDDWindow
{
// window message procedure
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	CDDWindow();
	~CDDWindow();

	static void InitWindow(HINSTANCE hInstance);
	
	// Create the window
	bool createWindow(const char *name,
					DWORD style=WS_OVERLAPPEDWINDOW, 
					int width=CW_USEDEFAULT, int height=0, int xPos=CW_USEDEFAULT, int yPos=0,
					HWND parent=0);
	// Create the window
	bool createWindowEx(DWORD exstyle, 
					const char *name,
					DWORD style=WS_OVERLAPPEDWINDOW, 
					int width=CW_USEDEFAULT, int height=0, int xPos=CW_USEDEFAULT, int yPos=0,
					HWND parent=0);	

	void setText(char *title);
	char *getText() const;

	// show up the window
	int show(int show=SW_SHOWDEFAULT) const;
	
	// repaint the window
	int update() const;

	// grab the DC
	HDC getDC() const;
	
	// release the DC safely
	void releaseDC(HDC hDC);

	// get the client rect area
	LPRECT getClientRect();
	
	DECLARE_MAP_MESSAGE()
public:
	HWND getWindowHandle() { return m_wndHandle; }
private:
	HWND m_wndHandle;
};

class CDDApplication : public CDDWindow
{
	DECLARE_MAP_MESSAGE()
	int OnDestory(DWORD wParam,DWORD lParam);
public:
	CDDApplication(HINSTANCE hInstance);
	virtual ~CDDApplication();
protected:
	virtual void afterCreate() {}
	virtual void beforeExit() {}
	virtual void idle() {}
	virtual void handleKey(float) = 0;
	virtual void printInfo() = 0;
	
	CTimer m_timer;

	float m_fps;
public:
	virtual bool createMainWindow();
	void run();
	void quit();
};

#endif 