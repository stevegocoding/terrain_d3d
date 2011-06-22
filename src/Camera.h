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

	/** ��ͼ�����ͶӰ���� */
	D3DXMATRIX			m_CameraMatrix;
	D3DXMATRIX			m_ProjMatrix;

	/** ŷ������ϵ */
	D3DXVECTOR3			m_vecLook;
	D3DXVECTOR3			m_vecRight;
	D3DXVECTOR3			m_vecUp;

	/** λ������ */
	D3DXVECTOR3			m_vecPosition;

	/** ƽ��̨��Ĳ��� */
	float				m_fFovY;
	float				m_fAspect;
	float				m_fNearClip;
	float				m_fFarClip;

	CDDFrustum*			m_pFrustum;
	bool				m_bProjCalcu;
	bool				m_bViewCalcu;

	CameraType			m_Type;

public:

	/** ��ȡ��Ϣ */
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

	/** ���¼�����ͼ��ͶӰ���� */
	void				CalculateView();
	void				CalculateProj();

public:

	/**  */
	void				Update();
	/** ����λ�� */
	void				SetPosition( D3DXVECTOR3* pos );
	void				SetPosition( float x ,float y ,float z );
	D3DXVECTOR3*		GetPosition();

	/** λ�� */
	void				Strafe( float units );
	void				Fly( float units );
	void				Walk( float units );

	/** ŷ��������ת */
	void				Pitch( float angle );
	void				Yaw( float angle );
	void				Roll( float angle );

};


#endif//_Camera_H_