#include "SkyBox.h"


void setNormals(SkyboxVertices &vertex,const D3DXVECTOR3 &normal) {
	vertex.nx = normal.x;
	vertex.ny = normal.y;
	vertex.nz = normal.z;
}


CDDSkyBox::CDDSkyBox(void)
{
	setCenter(D3DXVECTOR3(0,0,0));
	setSize(0.0f);
	for (int i= 0;i< NUMBER; i++) {
		m_textures[i] = 0;
	}
}

CDDSkyBox::~CDDSkyBox(void)
{
	for (int i= 0;i< NUMBER; i++) {
		SafeRelease(m_textures[i]);
	}
}


void CDDSkyBox::setCenter(const D3DXVECTOR3 &vecCenter)
{
	m_vecCenter = vecCenter;
}

void CDDSkyBox::setSize(float size)
{
	m_size = size;
	// top
	m_vertices[TOP][0].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[TOP][0].u = 0.0f;m_vertices[TOP][0].v = 0.0f;
	m_vertices[TOP][1].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[TOP][1].u = 1.0f;m_vertices[TOP][1].v = 0.0f;
	m_vertices[TOP][2].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[TOP][2].u = 0.0f;m_vertices[TOP][2].v = 1.0f;
	m_vertices[TOP][3].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[TOP][3].u = 1.0f;m_vertices[TOP][3].v = 1.0f;

	// bottom
	m_vertices[BOTTOM][0].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[BOTTOM][0].u = 0.0f;m_vertices[BOTTOM][0].v = 0.0f;
	m_vertices[BOTTOM][1].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[BOTTOM][1].u = 1.0f;m_vertices[BOTTOM][1].v = 0.0f;
	m_vertices[BOTTOM][2].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[BOTTOM][2].u = 0.0f;m_vertices[BOTTOM][2].v = 1.0f;
	m_vertices[BOTTOM][3].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[BOTTOM][3].u = 1.0f;m_vertices[BOTTOM][3].v = 1.0f;

	// left
	m_vertices[LEFT][0].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[LEFT][0].u = 0.0f;m_vertices[LEFT][0].v = 0.0f;
	m_vertices[LEFT][1].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[LEFT][1].u = 1.0f;m_vertices[LEFT][1].v = 0.0f;
	m_vertices[LEFT][2].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[LEFT][2].u = 0.0f;m_vertices[LEFT][2].v = 1.0f;
	m_vertices[LEFT][3].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[LEFT][3].u = 1.0f;m_vertices[LEFT][3].v = 1.0f;

	// right
	m_vertices[RIGHT][0].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[RIGHT][0].u = 0.0f;m_vertices[RIGHT][0].v = 0.0f;
	m_vertices[RIGHT][1].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[RIGHT][1].u = 1.0f;m_vertices[RIGHT][1].v = 0.0f;
	m_vertices[RIGHT][2].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[RIGHT][2].u = 0.0f;m_vertices[RIGHT][2].v = 1.0f;
	m_vertices[RIGHT][3].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[RIGHT][3].u = 1.0f;m_vertices[RIGHT][3].v = 1.0f;

	// back
	m_vertices[BACK][0].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[BACK][0].u = 0.0f;m_vertices[BACK][0].v = 0.0f;
	m_vertices[BACK][1].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  -m_size/2);m_vertices[BACK][1].u = 1.0f;m_vertices[BACK][1].v = 0.0f;
	m_vertices[BACK][2].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[BACK][2].u = 0.0f;m_vertices[BACK][2].v = 1.0f;
	m_vertices[BACK][3].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  -m_size/2);m_vertices[BACK][3].u = 1.0f;m_vertices[BACK][3].v = 1.0f;

	// front
	m_vertices[FRONT][0].pos = D3DXVECTOR3( - m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[FRONT][0].u = 0.0f;m_vertices[FRONT][0].v = 0.0f;
	m_vertices[FRONT][1].pos = D3DXVECTOR3( + m_size/2 ,  + m_size/2 ,  +m_size/2);m_vertices[FRONT][1].u = 1.0f;m_vertices[FRONT][1].v = 0.0f;
	m_vertices[FRONT][2].pos = D3DXVECTOR3( - m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[FRONT][2].u = 0.0f;m_vertices[FRONT][2].v = 1.0f;
	m_vertices[FRONT][3].pos = D3DXVECTOR3( + m_size/2 ,  - m_size/2 ,  +m_size/2);m_vertices[FRONT][3].u = 1.0f;m_vertices[FRONT][3].v = 1.0f;

	for (int i=0;i<6;i++) {
		for (int j = 0; j<4; j++) {
			setNormals(m_vertices[i][j],-1*m_vertices[i][j].pos);
		}
	}
}

bool CDDSkyBox::setTexture(SKYBOX_SIDE side,char *textureFileName)
{
	HRESULT hr = D3DXCreateTextureFromFile(GetRenderDevice().GetD3DDevice(),textureFileName,&m_textures[side]);
	
	if (FAILED (hr)) {
		return false; 
	}
	else return true;
}

void CDDSkyBox::render(void)
{
	// Set the world transform
	D3DXMATRIX pos;
	D3DXMatrixTranslation(&pos,m_vecCenter.x,m_vecCenter.y,m_vecCenter.z);
	GetRenderDevice().SetWorldMatrix(pos);

	// turn off the light
	//GetRenderDevice().GetD3DDevice()->SetRenderState(D3DRS_LIGHTING,false);
	//GetRenderDevice().GetD3DDevice()->LightEnable( 0 , false );

	
	//GetRenderDevice().SetDepthEnabled( false , true );
	GetRenderDevice().GetD3DDevice()->SetRenderState(D3DRS_ZENABLE,true);
	//GetRenderDevice().GetD3DDevice()->SetRenderState (D3DRS_ALPHABLENDENABLE, false);

	GetRenderDevice().GetD3DDevice()->SetFVF( SkyboxVertices::fvf );
	GetRenderDevice().GetD3DDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0 );
	GetRenderDevice().GetD3DDevice()->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_MODULATE );
	GetRenderDevice().GetD3DDevice()->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	GetRenderDevice().GetD3DDevice()->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	GetRenderDevice().GetD3DDevice()->SetTexture( 1 , NULL );

		// äÖÈ¾
	GetRenderDevice().SetTexture( 0 , m_textures[TOP] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[TOP] , sizeof(SkyboxVertices) );
	GetRenderDevice().SetTexture( 0 , m_textures[BOTTOM] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[BOTTOM] , sizeof(SkyboxVertices) );
	GetRenderDevice().SetTexture( 0 , m_textures[LEFT] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[LEFT] , sizeof(SkyboxVertices) );
	GetRenderDevice().SetTexture( 0 , m_textures[RIGHT] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[RIGHT] , sizeof(SkyboxVertices) );
	GetRenderDevice().SetTexture( 0 , m_textures[BACK] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[BACK] , sizeof(SkyboxVertices) );
	GetRenderDevice().SetTexture( 0 , m_textures[FRONT] );
	GetRenderDevice().GetD3DDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &m_vertices[FRONT] , sizeof(SkyboxVertices) );
}