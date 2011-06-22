#ifndef _Device_H_
#define	_Device_H_


#include "Prerequisites.h"


class CRenderDevice
{
public:

	CRenderDevice();
	~CRenderDevice();

public:

	/** 初始化设置，D3D初始化设备的时候需要这些设置
	 *	这里我们用一个Struct包容它们，方便统一编码。
	 */
	struct DevSettings
	{
		DevSettings();
		~DevSettings();

		// 是否全屏
		BOOL		bFullScreen;
		// 长宽
		UINT		unScreenWidth;
		UINT		unScreenHeight;
		// 挂接的Hwnd
		HWND		hWnd;
		// 屏幕像素格式
		D3DFORMAT	eBackBufferFormat;
		// 设备类型 HAL？REF？
		D3DDEVTYPE	eDevtype;
		// 设备性能 SOFTWARE？HARDWARE？
		DWORD		dwBehavior;
		// 是否等待垂直同步信号
		BOOL		bImmediate;

		void	RestoreDefault();
	};

private:

	// 设备当前属性设置
	DevSettings			m_Settings;

	// 设备
	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pDevice;

	// 当前用于渲染的Camera
	CCamera*			m_pCurrentCamera;

	// 用于Debug显示的子体对象
	LPD3DXFONT			m_pDebugFont;

private:

	/** 全局雾 */
	D3DFOGMODE			m_GlobalFogMode;
	D3DXCOLOR			m_GlobalFogColor;
	float				m_fGlobalFogStart;
	float				m_fGlobalFogEnd;
	float				m_fGlobalFogDensity;

public:

	/** 得到inner驱动指针 */
	inline LPDIRECT3D9	GetD3D()
	{ return m_pD3D; }
	/** 得到inner设备指针 */
	inline LPDIRECT3DDEVICE9	GetD3DDevice()
	{ return m_pDevice; }
	/** 获取设置 */
	inline const DevSettings&	GetSettings()
	{ return m_Settings; }

	/** 获取当前用来观察世界的摄像机 */
	inline	CCamera*	GetCurrentCamera()
	{ return m_pCurrentCamera; }

public:

	//////////////////////////////////////////////////////////////////////////
	/** 初始化和销毁 */
	bool				Initialize( const DevSettings& datas );
	void				Destroy();

	//////////////////////////////////////////////////////////////////////////
	/** 开始渲染 */
	bool				StartRender();

	/** 清屏 */
	void				ClearScreen( BYTE r , BYTE g , BYTE b );

	/** 结束渲染 */
	void				EndRender();

	//////////////////////////////////////////////////////////////////////////
	/** 输出Debug字符 */
	void				OutputDebugText( INT x , INT y , D3DCOLOR clr , const char* str , ... );

	//------------------------------------------------------------------------
	/** 设置摄像机 */
	void				SetCamera( CCamera* cam );
	/** 设置世界、视图、投影矩阵 */
	void				SetWorldMatrix( D3DXMATRIX& mat );
	void				SetViewMatrix( D3DXMATRIX& mat );
	void				SetProjMatrix( D3DXMATRIX& mat );

	void				GetViewMatrix( D3DXMATRIX& mat );
	void				GetProjMatrix( D3DXMATRIX& mat );

	//------------------------------------------------------------------------
	void				SetTexture( int slot , CDDTexture& tex );
	//void SetTexture(int slot, IDirect3DTexture9 *tex);

	//--------------------------------------------------------------------------------------------
	/** 设置光照 */
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
	/** 执行简单渲染操作
	*	@param pt			图元类型
	*	@param vb			CVertexBuffer的指针，存储有FVF格式
	*	@param ib			CIndexBuffer的指针
	*	@param vertexstart	从VertexBuffer的哪一个Vertex开始
	*	@param vertexcount	将要渲染VertexBuffer的多少VertexCount
	*	@param indexstart	从IndexBuffer的哪一个Index开始
	*	@param indexcount	将要渲染IndexBuffer的多少Index
	*/
	void				RenderFVF( D3DPRIMITIVETYPE pt , CDDVertexBuffer* vb , CDDIndexBuffer* ib , UINT vertexstart , UINT vertexcount , UINT indexstart , UINT indexcount );

};


inline CRenderDevice& GetRenderDevice()
{
	static CRenderDevice dev;
	return dev;
}

#endif//_Device_H_