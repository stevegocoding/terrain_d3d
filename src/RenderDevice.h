#ifndef _Device_H_
#define	_Device_H_


#include "Prerequisites.h"


class CRenderDevice
{
public:

	CRenderDevice();
	~CRenderDevice();

public:

	/** ��ʼ�����ã�D3D��ʼ���豸��ʱ����Ҫ��Щ����
	 *	����������һ��Struct�������ǣ�����ͳһ���롣
	 */
	struct DevSettings
	{
		DevSettings();
		~DevSettings();

		// �Ƿ�ȫ��
		BOOL		bFullScreen;
		// ����
		UINT		unScreenWidth;
		UINT		unScreenHeight;
		// �ҽӵ�Hwnd
		HWND		hWnd;
		// ��Ļ���ظ�ʽ
		D3DFORMAT	eBackBufferFormat;
		// �豸���� HAL��REF��
		D3DDEVTYPE	eDevtype;
		// �豸���� SOFTWARE��HARDWARE��
		DWORD		dwBehavior;
		// �Ƿ�ȴ���ֱͬ���ź�
		BOOL		bImmediate;

		void	RestoreDefault();
	};

private:

	// �豸��ǰ��������
	DevSettings			m_Settings;

	// �豸
	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pDevice;

	// ��ǰ������Ⱦ��Camera
	CCamera*			m_pCurrentCamera;

	// ����Debug��ʾ���������
	LPD3DXFONT			m_pDebugFont;

private:

	/** ȫ���� */
	D3DFOGMODE			m_GlobalFogMode;
	D3DXCOLOR			m_GlobalFogColor;
	float				m_fGlobalFogStart;
	float				m_fGlobalFogEnd;
	float				m_fGlobalFogDensity;

public:

	/** �õ�inner����ָ�� */
	inline LPDIRECT3D9	GetD3D()
	{ return m_pD3D; }
	/** �õ�inner�豸ָ�� */
	inline LPDIRECT3DDEVICE9	GetD3DDevice()
	{ return m_pDevice; }
	/** ��ȡ���� */
	inline const DevSettings&	GetSettings()
	{ return m_Settings; }

	/** ��ȡ��ǰ�����۲����������� */
	inline	CCamera*	GetCurrentCamera()
	{ return m_pCurrentCamera; }

public:

	//////////////////////////////////////////////////////////////////////////
	/** ��ʼ�������� */
	bool				Initialize( const DevSettings& datas );
	void				Destroy();

	//////////////////////////////////////////////////////////////////////////
	/** ��ʼ��Ⱦ */
	bool				StartRender();

	/** ���� */
	void				ClearScreen( BYTE r , BYTE g , BYTE b );

	/** ������Ⱦ */
	void				EndRender();

	//////////////////////////////////////////////////////////////////////////
	/** ���Debug�ַ� */
	void				OutputDebugText( INT x , INT y , D3DCOLOR clr , const char* str , ... );

	//------------------------------------------------------------------------
	/** ��������� */
	void				SetCamera( CCamera* cam );
	/** �������硢��ͼ��ͶӰ���� */
	void				SetWorldMatrix( D3DXMATRIX& mat );
	void				SetViewMatrix( D3DXMATRIX& mat );
	void				SetProjMatrix( D3DXMATRIX& mat );

	void				GetViewMatrix( D3DXMATRIX& mat );
	void				GetProjMatrix( D3DXMATRIX& mat );

	//------------------------------------------------------------------------
	void				SetTexture( int slot , CDDTexture& tex );
	//void SetTexture(int slot, IDirect3DTexture9 *tex);

	//--------------------------------------------------------------------------------------------
	/** ���ù��� */
	void				SetSceneAmbient( float r , float g , float b );
	void				SetLightEnabled( bool tag );
	//------------------------------------------------------------------------
	void				SetFog( D3DFOGMODE mode , float r , float g , float b , float start , float end , float densitiy );
	void				SetFog( D3DFOGMODE mode , D3DCOLORVALUE color , float start , float end , float densitiy );
	void				SetGlobalFog( D3DFOGMODE mode , D3DCOLORVALUE color , float start , float end , float densitiy );
	void				UseGlobalFog( bool setting = true );
	inline void			DisableFog(){ SetFog( D3DFOG_NONE , 0 , 0 , 0 , 0 , 0 , 0 ); }

	void				SetDepthEnabled( bool read , bool write );
	//------------------------------------------------------------------------
	void				SetSceneBlend( D3DBLEND src , D3DBLEND dst );
	void				DisableSceneBlend();
	//------------------------------------------------------------------------
	void				SetTextureColorOp( UCHAR slot , D3DTEXTUREOP op , UINT arg1 , UINT arg2 );
	void				SetTextureAlphaOp( UCHAR slot , D3DTEXTUREOP op , UINT arg1 , UINT arg2 );
	void				DisableTextureStage( UCHAR slot );

	void				SetShadeMode( D3DSHADEMODE mode );
	void				SetCullMode( D3DCULL cull );
	/** ִ�м���Ⱦ����
	*	@param pt			ͼԪ����
	*	@param vb			CVertexBuffer��ָ�룬�洢��FVF��ʽ
	*	@param ib			CIndexBuffer��ָ��
	*	@param vertexstart	��VertexBuffer����һ��Vertex��ʼ
	*	@param vertexcount	��Ҫ��ȾVertexBuffer�Ķ���VertexCount
	*	@param indexstart	��IndexBuffer����һ��Index��ʼ
	*	@param indexcount	��Ҫ��ȾIndexBuffer�Ķ���Index
	*/
	void				RenderFVF( D3DPRIMITIVETYPE pt , CDDVertexBuffer* vb , CDDIndexBuffer* ib , UINT vertexstart , UINT vertexcount , UINT indexstart , UINT indexcount );

};


inline CRenderDevice& GetRenderDevice()
{
	static CRenderDevice dev;
	return dev;
}

#endif//_Device_H_