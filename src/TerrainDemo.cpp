// 纹理 Demo

#include "DemoApp.h"
#include "TerrainDemo.h"
#include "RenderDevice.h"

BEGIN_MAP_MESSAGE(MyApplication,CDDApplication)
	MAP_MESSAGE(WM_KEYDOWN,MyApplication::onKeyDown)
END_MAP_MESSAGE()

MyApplication::MyApplication(HINSTANCE hInstance) : CDDApplication(hInstance)
{

}

bool MyApplication::createMainWindow()
{
	if (!createWindow("Interlock Tiles QTree Terrain Demo") )
		return false;
	if (!initD3D(800,600)) 
		return false;
	
	return true;
}

void MyApplication::beforeExit()
{
	m_terrain.Destroy();
}


bool MyApplication::initD3D(int width, int height)
{
	CRenderDevice::DevSettings setting;
	setting.hWnd = getWindowHandle();
	setting.unScreenHeight = height;
	setting.unScreenWidth = width;
	setting.eBackBufferFormat = D3DFMT_A8R8G8B8;
	setting.dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Init Direct3D
	GetRenderDevice().Initialize(setting);

	return true;
}

void MyApplication::afterCreate()
{

}

void MyApplication::setupScene()
{
	m_camera.SetPosition(&D3DXVECTOR3(0,500,0));
	
	GetRenderDevice().SetCamera(&m_camera);
	
	D3DXMATRIX world;
	D3DXMatrixTranslation(&world,0.0f,0.0f,0.0f);
	GetRenderDevice().SetWorldMatrix(world);

	m_terrain.Initialize(0);
}

void MyApplication::idle()
{
	static float last_time = (float)timeGetTime();

	float curr_time = (float)timeGetTime();
	float time_delta =  (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	// 响应按键操作
	handleKey(time_delta);
	
	GetRenderDevice().ClearScreen(0,0,255);
	GetRenderDevice().StartRender();
	
	m_terrain.UpdateFrame();
	m_terrain.Render();
	

	printInfo();

	GetRenderDevice().EndRender();
}

void  MyApplication::handleKey(float time_delta)
{
	int stride = 200;
	// press 'W'
	if ( ::GetAsyncKeyState('W') & 0x8000f )
		m_camera.Walk( time_delta * stride);
	// press 'S'
	if ( ::GetAsyncKeyState('S') & 0x8000f )
		m_camera.Walk( time_delta * stride * (-1));
	
	if ( ::GetAsyncKeyState('A') & 0x8000f )
		m_camera.Strafe(time_delta * stride * (-1));
	
	if ( ::GetAsyncKeyState('D') & 0x8000f )
		m_camera.Strafe(time_delta * stride);
	
	if ( ::GetAsyncKeyState(VK_LEFT ) & 0x8000f )
		m_camera.Yaw( time_delta * stride * 0.3 * -1);

	if ( ::GetAsyncKeyState(VK_RIGHT ) & 0x8000f )
		m_camera.Yaw( time_delta * stride * 0.3);

	if ( ::GetAsyncKeyState(VK_DOWN ) & 0x8000f )
		m_camera.Pitch( time_delta * stride * 0.3);

	if ( ::GetAsyncKeyState(VK_UP ) & 0x8000f )
		m_camera.Pitch( time_delta * stride * 0.3 * -1);
}

int MyApplication::onKeyDown(DWORD wParam,DWORD lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		quit();
		break;
	default:
		break;
	}

	return 1;
}

void MyApplication::printInfo()
{
	GetRenderDevice().OutputDebugText(10,10,0xff00ff00,"FPS: %4.2f",m_fps);
}


// Main function
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)		
{
	MyApplication app(hInstance);
	bool result = app.createMainWindow();
	app.setupScene();
	app.run();
	app.beforeExit();
	return 1;
}