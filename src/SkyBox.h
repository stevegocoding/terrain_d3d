#ifndef _D3D_SKYBOX_H_
#define _D3D_SKYBOX_H_

#include "prerequisites.h"
#include "Device.h"


//------------------------------------------------------------------------
// Custom Vertex：位置、顶点颜色、纹理坐标
struct SkyboxVertices{
	enum{
		fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		//fvf = D3DFVF_XYZ | D3DFVF_TEX1,
	};

	D3DVECTOR	pos;
	float		nx,ny,nz;
	float		u;
	float		v;
	
};
//------------------------------------------------------------------------

class CDDSkyBox
{
public:
	enum SKYBOX_SIDE
	{
		TOP	= 0 ,
		BOTTOM	,
		LEFT	,
		RIGHT	,
		BACK	,
		FRONT	,
		NUMBER	,
	};

	CDDSkyBox(void);
	~CDDSkyBox(void);
	void render(void);
	void setSize(float size);
	void setCenter(const D3DXVECTOR3 &verCenter);
	bool setTexture(SKYBOX_SIDE side,char *textureFileName);

private:
	float m_size;
	D3DXVECTOR3 m_vecCenter;
	IDirect3DTexture9 *m_textures[NUMBER];
	SkyboxVertices m_vertices[6][4];
};







#endif