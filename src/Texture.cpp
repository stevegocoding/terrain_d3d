#include "Texture.h"
#include "RenderDevice.h"
#include "Image.h"

CDDTexture::CDDTexture()
{
	m_pTexture = 0;
}

CDDTexture::~CDDTexture()
{
	destroy();
}


bool CDDTexture::createFromFile(const char *fileName)
{	
	// �����٣��ٴ���
	destroy();

	HRESULT hr;
	if ( FAILED( hr = D3DXCreateTextureFromFile(GetRenderDevice().GetD3DDevice(),fileName,&m_pTexture) ) ) {
		return false;
	}

	return true;
}

/* 
1. �����ڴ������ݣ������������棬��������
2. �õ�texture�ĵ�0�����ָ��
3. �Ӹոմ������������濽�����ݵ�texture�еı���
*/
bool CDDTexture::createFromMemory(BYTE *pData,
								unsigned int width,
								unsigned int height,
								D3DFORMAT format,
								D3DTEXTUREFILTERTYPE mipFilter)
{
	destroy();
	
	HRESULT hr;
	D3DFORMAT d3dPF = D3DFMT_A8R8G8B8;

	hr = D3DXCreateTexture(GetRenderDevice().GetD3DDevice(),width,height,0,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTexture);
	if ( FAILED(hr) ) {
		destroy();
		return false;
	}

	RECT tempDataRect = {0,0,width,height};

	// ������������
	IDirect3DSurface9 *pSrcSurface = 0;
	hr = GetRenderDevice().GetD3DDevice()->CreateOffscreenPlainSurface(width,height,d3dPF,D3DPOOL_SCRATCH,&pSrcSurface,0);
	if ( FAILED(hr) ) {
		destroy();
		return false;
	}

	// ��������
	D3DXLoadSurfaceFromMemory(pSrcSurface,0,&tempDataRect,pData,format,width * GetD3DFormatLength(format),NULL,&tempDataRect,D3DX_DEFAULT,0);
	if ( FAILED(hr) )
	{
		SafeRelease( pSrcSurface );
		destroy();
		return false;
	}

	// �ҵ�Mip0ҳ��
	IDirect3DSurface9 *pDstSurface; 
	hr = m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
	if (FAILED(hr))
	{
		SafeRelease( pSrcSurface );
		destroy();
		return false;
	}

	// ����ҳ��
	hr = D3DXLoadSurfaceFromSurface( pDstSurface, NULL, NULL, pSrcSurface, NULL, NULL, D3DX_DEFAULT, 0 );
	if (FAILED(hr))
	{
		SafeRelease( pSrcSurface );
		SafeRelease( pDstSurface );
		destroy();
		return false;
	}

	// �Զ�mip����
	hr = m_pTexture->SetAutoGenFilterType(mipFilter);
	if (FAILED (hr)) {
		SafeRelease( pSrcSurface );
		SafeRelease( pDstSurface );
		destroy();
		return false;
	}

	m_pTexture->GenerateMipSubLevels();

	SafeRelease( pSrcSurface );
	SafeRelease( pDstSurface );

	return true;
}

void CDDTexture::destroy()
{
	SafeRelease(m_pTexture);
}