#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "Prerequisites.h"

#define COMPLETE_OUT 0
#define INTERSECT    1
#define COMPLETE_IN  2

class CDDFrustum
{
public:
	CDDFrustum()
	{}
	~CDDFrustum()
	{}

protected:

	// This holds the A B C and D values for each side of our frustum.
	D3DXPLANE	m_Frustum[6];

public:

	// Call this every time the camera moves to update the frustum
	void		CalculateFrustum( D3DXMATRIX* modl , D3DXMATRIX* proj );
	// This takes a 3D point and returns TRUE if it's inside of the frustum
	bool		PointInFrustum(float x, float y, float z);
	// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
	bool		SphereInFrustum(float x, float y, float z, float radius);
	// This takes the center and half the length of the cube.
	bool		CubeInFrustum(float x, float y, float z, float size);
	// This checks if a box is in the frustum
	int			BoxInFrustum(float x, float y, float z, float x2, float y2, float z2);

};

#endif