#include "RenderDevice.h"
#include "Camera.h"
#include "VertexIndexBuffer.h"
#include "texture.h"


//////////////////////////////////////////////////////////////////////////
CRenderDevice::DevSettings::DevSettings()
{
	bFullScreen = false;
	unScreenWidth = 0;
	unScreenHeight = 0;
	hWnd = NULL;
	eBackBufferFormat = D3DFMT_UNKNOWN;
	eDevtype = D3DDEVTYPE_HAL;
	dwBehavior = 0;
	bImmediate = TRUE;
}

//////////////////////////////////////////////////////////////////////////
CRenderDevice::DevSettings::~DevSettings()
{
	// do nothing
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::DevSettings::RestoreDefault()
{
	bFullScreen = false;
	unScreenWidth = 800;
	unScreenHeight = 600;
	eBackBufferFormat = D3DFMT_X8R8G8B8;
	eDevtype = D3DDEVTYPE_HAL;
	dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	bImmediate = TRUE;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
CRenderDevice::CRenderDevice() : m_pD3D(NULL) , m_pDevice(NULL) , m_pDebugFont(NULL)
{
	m_GlobalFogMode = D3DFOG_NONE;
	m_GlobalFogColor = D3DXCOLOR( 1.0f , 1.0f , 1.0f , 1.0f );
	m_fGlobalFogStart = 1000.0f;
	m_fGlobalFogEnd = 5000.0f;
	m_fGlobalFogDensity = 0.0008f;
}

//////////////////////////////////////////////////////////////////////////
CRenderDevice::~CRenderDevice()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////
bool	CRenderDevice::Initialize( const DevSettings& datas )
{
	// 已经建立，那么应先销毁-------------------------------------------------------
	if ( m_pDevice )
	{ Destroy(); }

	// 改变外部窗口的几何属性-------------------------------------------------------
	RECT rct;
	// 获取窗口的大小
	::GetWindowRect( datas.hWnd , &rct );
	// 改变大小
	rct.right = rct.left + datas.unScreenWidth;
	rct.bottom = rct.top + datas.unScreenHeight;
	// 重设窗口
	WINDOWINFO info;
	info.cbSize = sizeof(WINDOWINFO);
	if ( !::GetWindowInfo( datas.hWnd , &info ) )
	{ return false; }
	if( !::AdjustWindowRectEx( &rct , info.dwStyle , (::GetMenu(datas.hWnd) != NULL) , info.dwExStyle ) )
	{ return false; }
	::MoveWindow( datas.hWnd , 0 , 0 , rct.right - rct.left , rct.bottom - rct.top , TRUE );

	// 生成Driver-------------------------------------------------------
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{ return false; }

	// 读取设置
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed					= !datas.bFullScreen;
	d3dpp.SwapEffect				= D3DSWAPEFFECT_FLIP;
	d3dpp.BackBufferCount			= 1;
	d3dpp.hDeviceWindow				= datas.hWnd;
	d3dpp.BackBufferWidth			= datas.unScreenWidth;
	d3dpp.BackBufferHeight			= datas.unScreenHeight;
	d3dpp.BackBufferFormat			= datas.eBackBufferFormat;
	d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	if ( datas.bImmediate )
	{ d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE; }
	else
	{ d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT; }

	HRESULT hr;

	// 生成Device-------------------------------------------------------
	if( FAILED( hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT , datas.eDevtype , datas.hWnd , datas.dwBehavior, &d3dpp, &m_pDevice ) ) )
	{
		const char* error = DXGetErrorDescription( hr );

		if ( datas.eDevtype == D3DDEVTYPE_HAL )
		{
			// 如果刚刚用硬件设备生成的，那么这里尝试用参考设备再生成一次
			if ( FAILED( hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , datas.hWnd , datas.dwBehavior, &d3dpp, &m_pDevice ) ) )
			{
				const char* error = DXGetErrorDescription( hr );

				return false;
			}
		}
		// 如果本来就是参考设备，那么这里也不必再生成一次了，挂吧！
		else
		{ return false; }
	}

	m_Settings = datas;

	// 生成DebugFont-------------------------------------------------------
	D3DXFONT_DESC fd;
	memset( &fd, 0, sizeof(D3DXFONT_DESC) );		// zero out structure
	fd.CharSet = GB2312_CHARSET;
	fd.Height = 12;
	strcpy( fd.FaceName, "Arial" );

	hr = D3DXCreateFontIndirect( m_pDevice , &fd , &m_pDebugFont );

	return true;
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::Destroy()
{
	SafeRelease(m_pDebugFont);
	SafeRelease(m_pDevice);
	SafeRelease(m_pD3D);
}

//////////////////////////////////////////////////////////////////////////
bool	CRenderDevice::StartRender()
{
	if( FAILED( m_pDevice->BeginScene() ) )
	{ return false; }

	if ( m_pCurrentCamera )
	{ m_pCurrentCamera->Update(); }

	return true;
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::ClearScreen( BYTE r , BYTE g , BYTE b )
{
	m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB( r , g , b ) , 1.0f , 0 );
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::EndRender()
{
	// 交换BackBuffer到屏幕
	m_pDevice->EndScene();
	m_pDevice->Present( NULL, NULL, NULL, NULL );
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::OutputDebugText( INT x , INT y , D3DCOLOR clr , const char* str , ... )
{
	static	char	szBuffer[512];
	
	va_list	list;
	va_start(list , str);
	vsprintf( szBuffer , str , list);
	va_end(list);
	/*
	::memset(szBuffer,'\0',512);
	strcpy(szBuffer,str);
	*/
	RECT rct;
	rct.left = x;
	rct.top = y;
	rct.right = x + 10;
	rct.bottom = y + 10;
	m_pDebugFont->DrawText( NULL , szBuffer , strlen(szBuffer) , &rct , DT_NOCLIP | DT_LEFT | DT_TOP , clr );
}

//------------------------------------------------------------------------
void	CRenderDevice::SetCamera( CCamera* cam )
{
	m_pCurrentCamera = cam;
}

//------------------------------------------------------------------------
void	CRenderDevice::SetWorldMatrix( D3DXMATRIX& mat )
{
	m_pDevice->SetTransform( D3DTS_WORLD , &mat );
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::SetViewMatrix( D3DXMATRIX& mat )
{
	m_pDevice->SetTransform( D3DTS_VIEW , &mat );
}

//////////////////////////////////////////////////////////////////////////
void	CRenderDevice::SetProjMatrix( D3DXMATRIX& mat )
{
	m_pDevice->SetTransform( D3DTS_PROJECTION , &mat );
}

//------------------------------------------------------------------------
void	CRenderDevice::GetViewMatrix( D3DXMATRIX& mat )
{
	m_pDevice->GetTransform( D3DTS_VIEW , &mat );
}

//------------------------------------------------------------------------
void	CRenderDevice::GetProjMatrix( D3DXMATRIX& mat )
{
	m_pDevice->GetTransform( D3DTS_PROJECTION , &mat );
}


//------------------------------------------------------------------------
void	CRenderDevice::SetTexture( int slot , CDDTexture& tex )
{
	m_pDevice->SetTexture( slot , tex.getD3DTexture() );
}


//void CRenderDevice::SetTexture(int slot, IDirect3DTexture9 *tex)
//{
//	m_pDevice->SetTexture( slot , tex );
//}

//------------------------------------------------------------
void	CRenderDevice::SetSceneAmbient( float r , float g , float b )
{
	// 设置RenderState的Ambient
	m_pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( r, g, b, 1.0f ) );
}

//------------------------------------------------------------
void	CRenderDevice::SetLightEnabled( bool tag )
{
	m_pDevice->SetRenderState( D3DRS_LIGHTING, tag );
}

//------------------------------------------------------------
void	CRenderDevice::SetFog( D3DFOGMODE mode , float r , float g , float b , float start , float end , float densitiy )
{
	HRESULT hr;

	if( mode == D3DFOG_NONE )
	{
		hr = m_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE , D3DFOG_NONE );
		hr = m_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	}
	else
	{
		hr = m_pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		hr = m_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE , mode );

		hr = m_pDevice->SetRenderState( D3DRS_FOGCOLOR , D3DCOLOR_COLORVALUE( r , g , b , 1.0f ) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGSTART, *((LPDWORD)(&start)) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGEND, *((LPDWORD)(&end)) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGDENSITY, *((LPDWORD)(&densitiy)) );
	}

	if( FAILED( hr ) )
	{
		const char* error = DXGetErrorDescription(hr );
	}
}

//------------------------------------------------------------
void	CRenderDevice::SetFog( D3DFOGMODE mode , D3DCOLORVALUE color , float start , float end , float densitiy )
{
	HRESULT hr;

	if( mode == D3DFOG_NONE )
	{
		hr = m_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE , D3DFOG_NONE );
		hr = m_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	}
	else
	{
		hr = m_pDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		hr = m_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE , mode );

		hr = m_pDevice->SetRenderState( D3DRS_FOGCOLOR , D3DCOLOR_COLORVALUE( color.r , color.g , color.b , 1.0f ) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGSTART, *((LPDWORD)(&start)) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGEND, *((LPDWORD)(&end)) );
		hr = m_pDevice->SetRenderState( D3DRS_FOGDENSITY, *((LPDWORD)(&densitiy)) );
	}

	if( FAILED( hr ) )
	{
		const char* error = DXGetErrorDescription( hr );
	}
}

//------------------------------------------------------------
void	CRenderDevice::SetGlobalFog( D3DFOGMODE mode , D3DCOLORVALUE color , float start , float end , float densitiy )
{
	m_GlobalFogMode = mode;
	m_GlobalFogColor = color;
	m_fGlobalFogStart = start;
	m_fGlobalFogEnd = end;
	m_fGlobalFogDensity = densitiy;
}

//------------------------------------------------------------
void	CRenderDevice::UseGlobalFog( bool setting /* = true */ )
{
	if ( setting )
	{ SetFog( m_GlobalFogMode , m_GlobalFogColor , m_fGlobalFogStart , m_fGlobalFogEnd , m_fGlobalFogDensity ); }
	else
	{ DisableFog(); }
}

//------------------------------------------------------------------------
void	CRenderDevice::SetDepthEnabled( bool read , bool write )
{
	m_pDevice->SetRenderState( D3DRS_ZENABLE , read );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE , write );
}

//------------------------------------------------------------
void	CRenderDevice::SetSceneBlend( D3DBLEND src , D3DBLEND dst )
{
	if ( src != D3DBLEND_ONE || dst != D3DBLEND_ZERO )
	{ m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , true ); }
	else
	{ m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , false ); }
	m_pDevice->SetRenderState( D3DRS_SRCBLEND , src );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND , dst );
}

//------------------------------------------------------------
void	CRenderDevice::DisableSceneBlend()
{
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , false );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ONE );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_ZERO );
}

//------------------------------------------------------------------------
void	CRenderDevice::SetTextureColorOp( UCHAR slot , D3DTEXTUREOP op , UINT arg1 , UINT arg2 )
{
	m_pDevice->SetTextureStageState( slot , D3DTSS_COLOROP,   op );
	m_pDevice->SetTextureStageState( slot , D3DTSS_COLORARG1, arg1 );
	m_pDevice->SetTextureStageState( slot , D3DTSS_COLORARG2, arg2 );
}

//------------------------------------------------------------------------
void	CRenderDevice::SetTextureAlphaOp( UCHAR slot , D3DTEXTUREOP op , UINT arg1 , UINT arg2 )
{
	m_pDevice->SetTextureStageState( slot , D3DTSS_ALPHAOP,   op );
	m_pDevice->SetTextureStageState( slot , D3DTSS_ALPHAARG1, arg1 );
	m_pDevice->SetTextureStageState( slot , D3DTSS_ALPHAARG2, arg2 );
}

//------------------------------------------------------------------------
void	CRenderDevice::DisableTextureStage( UCHAR slot )
{
	if ( slot == 0 )
	{ SetTextureColorOp( slot , D3DTOP_MODULATE , D3DTA_TEXTURE , D3DTA_DIFFUSE ); }
	else
	{ SetTextureColorOp( slot , D3DTOP_DISABLE , D3DTA_TEXTURE , D3DTA_CURRENT ); }
}

//------------------------------------------------------------------------
void	CRenderDevice::SetShadeMode( D3DSHADEMODE mode )
{
	m_pDevice->SetRenderState( D3DRS_SHADEMODE , mode );
}

//------------------------------------------------------------------------
void	CRenderDevice::SetCullMode( D3DCULL cull )
{
	m_pDevice->SetRenderState( D3DRS_CULLMODE , cull );
}


//------------------------------------------------------------------------
void	CRenderDevice::RenderFVF( D3DPRIMITIVETYPE pt , CDDVertexBuffer* vb , CDDIndexBuffer* ib , UINT vertexstart , UINT vertexcount , UINT indexstart , UINT indexcount )
{
	m_pDevice->SetStreamSource( 0 , vb->m_pVertexBuffer , 0 , vb->m_stride );
	m_pDevice->SetFVF( vb->m_vertexFormat );

	// 计算图元数量，有索引，按照索引的数量，没有索引，按照顶点的数量
	UINT primCount = 0;
	switch( pt )
	{
	case D3DPT_POINTLIST:
		primCount = (ib ? indexcount : vertexcount );
		break;
	case D3DPT_LINELIST:
		primCount = (ib ? indexcount : vertexcount ) / 2;
		break;
	case D3DPT_LINESTRIP:
		primCount = (ib ? indexcount : vertexcount ) - 1;
		break;
	case D3DPT_TRIANGLELIST:
		primCount = (ib ? indexcount : vertexcount ) / 3;
		break;
	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		primCount = (ib ? indexcount : vertexcount ) - 2;
		break;
	}

	if (!primCount)
		return;

	HRESULT hr;
	// 有索引就使用索引
	if ( ib )
	{
		m_pDevice->SetIndices( ib->m_pIndexBuffer );
		hr = m_pDevice->DrawIndexedPrimitive( pt , vertexstart , 0 , vertexcount , indexstart , primCount );
	}
	// 否则使用VB
	else
	{
		hr = m_pDevice->DrawPrimitive( pt , vertexstart , primCount ); 
	}
	if ( FAILED(hr) )
	{ const char* error = DXGetErrorDescription( hr ); }
}



