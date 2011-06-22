#ifndef _D3D_TEXTURE_H_
#	define _D3D_TEXTURE_H_

#include "ITTerrain.h"
#include "camera.h"

class MyApplication : public CDDApplication
{
	DECLARE_MAP_MESSAGE()
public:
	MyApplication(HINSTANCE hInstance);
	~MyApplication() { }
	bool createMainWindow();
	bool initD3D(int,int);
	void setupScene();

	void afterCreate();
	void beforeExit();
	void idle();
	void handleKey(float time_delta);
	int onKeyDown(DWORD wParam,DWORD lParam);
	void printInfo();

private:
	// ÉãÏñ»ú
	CCamera m_camera;
	CDDITTerrain m_terrain;
	
	
	


};



#endif