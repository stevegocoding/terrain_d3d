#include "Camera.h"
#include "RenderDevice.h"
#include "Frustum.h"

//------------------------------------------------------------
CCamera::CCamera( CameraType ct /* = CT_FREE */ )
: m_Type(ct) , m_vecRight(1.0f , 0.0f , 0.0f) , m_vecUp(0.0f , 1.0f , 0.0f), m_vecLook(0.0f , 0.0f , 1.0f)
{
	D3DXVec3Normalize(&m_vecRight, &m_vecRight);
	D3DXVec3Normalize(&m_vecUp, &m_vecUp);
	D3DXVec3Normalize(&m_vecLook, &m_vecLook);

	m_fFovY = D3DX_PI / 2 ;
	m_fAspect = 4.0f / 3.0f ;

	m_fNearClip = 1.0f;
	m_fFarClip = 1000.0f;

	m_bViewCalcu = true;
	m_bProjCalcu = true;


	m_pFrustum = new CDDFrustum;
}

//------------------------------------------------------------
CCamera::~CCamera()
{
	SafeDelete(m_pFrustum);
}

//------------------------------------------------------------
void CCamera::Update()
{
	static bool updateFrustum = false;
	if ( m_bViewCalcu )
	{
		CalculateView();
		GetRenderDevice().SetViewMatrix( m_CameraMatrix );
		updateFrustum = true;
	}
	if ( m_bProjCalcu )
	{
		CalculateProj();
		GetRenderDevice().SetProjMatrix( m_ProjMatrix );
		updateFrustum = true;
	}

	
	if ( updateFrustum )
	{
		m_pFrustum->CalculateFrustum( &m_CameraMatrix , &m_ProjMatrix );
		updateFrustum = false;
	}
	
}

//------------------------------------------------------------
D3DXVECTOR3*	CCamera::GetPosition()
{
	return &m_vecPosition;
}

//------------------------------------------------------------
void			CCamera::CalculateView()
{	
	// 保证摄像机的几个轴相互垂直：
	D3DXVec3Normalize(&m_vecLook, &m_vecLook);
	D3DXVec3Cross(&m_vecUp, &m_vecLook, &m_vecRight);
	D3DXVec3Normalize(&m_vecUp, &m_vecUp);
	D3DXVec3Cross(&m_vecRight, &m_vecUp, &m_vecLook);
	D3DXVec3Normalize(&m_vecRight, &m_vecRight);
	// 建立视图矩阵：
	float x = -D3DXVec3Dot(&m_vecRight, &m_vecPosition);
	float y = -D3DXVec3Dot(&m_vecUp, &m_vecPosition);
	float z = -D3DXVec3Dot(&m_vecLook, &m_vecPosition);
	m_CameraMatrix(0, 0) = m_vecRight.x;
	m_CameraMatrix(0, 1) = m_vecUp.x;
	m_CameraMatrix(0, 2) = m_vecLook.x;
	m_CameraMatrix(0, 3) = 0.0f;
	m_CameraMatrix(1, 0) = m_vecRight.y;
	m_CameraMatrix(1, 1) = m_vecUp.y;
	m_CameraMatrix(1, 2) = m_vecLook.y;
	m_CameraMatrix(1, 3) = 0.0f;
	m_CameraMatrix(2, 0) = m_vecRight.z;
	m_CameraMatrix(2, 1) = m_vecUp.z;
	m_CameraMatrix(2, 2) = m_vecLook.z;
	m_CameraMatrix(2, 3) = 0.0f;
	m_CameraMatrix(3, 0) = x;
	m_CameraMatrix(3, 1) = y;
	m_CameraMatrix(3, 2) = z;
	m_CameraMatrix(3, 3) = 1.0f;

	
	m_bViewCalcu = false;
}

//------------------------------------------------------------
void			CCamera::CalculateProj()
{
	D3DXMatrixPerspectiveFovLH( &m_ProjMatrix, m_fFovY , m_fAspect , m_fNearClip , m_fFarClip );

	m_bProjCalcu = false;
}

//------------------------------------------------------------
void			CCamera::SetPosition( D3DXVECTOR3* pos )
{
	m_vecPosition = *pos;
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::SetPosition( float x ,float y ,float z )
{
	m_vecPosition = D3DXVECTOR3( x , y , z );
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Strafe( float units )
{
	if( m_Type == CT_LAND )
	{
		m_vecPosition += D3DXVECTOR3(m_vecRight.x, 0.0f, m_vecRight.z) * units;
	}
	else if ( m_Type == CT_FREE )
	{
		m_vecPosition += m_vecRight * units;
	}
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Fly( float units )
{
	if ( m_Type == CT_LAND )
	{
		// Land只允许平行的上下移动
		m_vecPosition += D3DXVECTOR3(0,1,0) * units;
	}
	else if ( m_Type == CT_FREE )
	{
		m_vecPosition += m_vecUp * units;
	}
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Walk( float units )
{
	if( m_Type == CT_LAND )
	{
		m_vecPosition += D3DXVECTOR3(m_vecLook.x, 0.0f, m_vecLook.z) * units;
	}
	else if ( m_Type == CT_FREE )
	{
		m_vecPosition += m_vecLook * units;
	}
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Pitch( float angle )
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vecRight, angle);
	// 参照m_vecRight矢量旋转
	D3DXVec3TransformCoord(&m_vecUp,&m_vecUp, &T);
	D3DXVec3TransformCoord(&m_vecLook,&m_vecLook, &T);
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Yaw( float angle )
{
	D3DXMATRIX T;
	if( m_Type == CT_LAND )
	{
		// 陆地模式仅允许按照世界Y轴旋转
		D3DXMatrixRotationY(&T, angle);
	}
	else if( m_Type == CT_FREE )
	{
		// 否则就按照up矢量旋转
		D3DXMatrixRotationAxis(&T, &m_vecUp, angle);
	}

	// 参照m_vecUp旋转
	D3DXVec3TransformCoord(&m_vecRight,&m_vecRight, &T);
	D3DXVec3TransformCoord(&m_vecLook,&m_vecLook, &T);
	m_bViewCalcu = true;
}

//------------------------------------------------------------
void			CCamera::Roll( float angle )
{
	// 陆地模式不允许倾斜
	if ( m_Type == CT_LAND )
	{ return; }

	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vecLook, angle);
	// 参照m_vecLook旋转
	D3DXVec3TransformCoord(&m_vecRight,&m_vecRight, &T);
	D3DXVec3TransformCoord(&m_vecUp,&m_vecUp, &T);
	m_bViewCalcu = true;
}