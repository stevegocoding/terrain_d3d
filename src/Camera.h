#ifndef _Camera_H_
#define	_Camera_H_


#include "prerequisites.h"

enum CameraType
{
	CT_FREE		= 0 ,
	CT_LAND		= 1 ,
};

class CCamera
{
public:

	CCamera( CameraType ct = CT_FREE );
	~CCamera();

private:

	/** 视图矩阵和投影矩阵 */
	D3DXMATRIX			m_CameraMatrix;
	D3DXMATRIX			m_ProjMatrix;

	/** 欧拉坐标系 */
	D3DXVECTOR3			m_vecLook;
	D3DXVECTOR3			m_vecRight;
	D3DXVECTOR3			m_vecUp;

	/** 位置坐标 */
	D3DXVECTOR3			m_vecPosition;

	/** 平截台体的参数 */
	float				m_fFovY;
	float				m_fAspect;
	float				m_fNearClip;
	float				m_fFarClip;

	CDDFrustum*			m_pFrustum;
	bool				m_bProjCalcu;
	bool				m_bViewCalcu;

	CameraType			m_Type;

public:

	/** 获取信息 */
	inline const D3DXVECTOR3&	GetLookVector()
	{ return m_vecLook; }
	inline const D3DXVECTOR3&	GetUpVector()
	{ return m_vecUp; }
	inline const D3DXVECTOR3&	GetRightVector()
	{ return m_vecRight; }

	inline D3DXMATRIX*	GetViewMatrix()
	{ return &m_CameraMatrix; }
	inline D3DXMATRIX*	GetProjMatrix()
	{ return &m_ProjMatrix; }

	inline void	SetFarClip( float clip ) { 
		m_fFarClip = clip ; 
		m_bProjCalcu = true; 
	}

	inline	void		SetFovAngle( float degree )
	{ SetFovRadian(D3DXToRadian(degree)); }
	inline	void		SetFovRadian( float r )
	{ m_fFovY = r; m_bProjCalcu = true; }

	inline	float		GetFovAngle()
	{ return D3DXToDegree(m_fFovY); }
	inline	float		GetFovRadian()
	{ return m_fFovY; }

	
	inline CDDFrustum*	GetFrustum()
	{ return m_pFrustum; }
	
private:

	/** 重新计算视图和投影矩阵 */
	void				CalculateView();
	void				CalculateProj();

public:

	/**  */
	void				Update();
	/** 设置位置 */
	void				SetPosition( D3DXVECTOR3* pos );
	void				SetPosition( float x ,float y ,float z );
	D3DXVECTOR3*		GetPosition();

	/** 位移 */
	void				Strafe( float units );
	void				Fly( float units );
	void				Walk( float units );

	/** 欧拉坐标旋转 */
	void				Pitch( float angle );
	void				Yaw( float angle );
	void				Roll( float angle );

};


#endif//_Camera_H_