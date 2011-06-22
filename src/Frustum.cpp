#include "Frustum.h"


D3DXVECTOR3	g_StandFrustum[8] =
{
	D3DXVECTOR3(-1.0f, -1.0f,  0.0f), // xyz
	D3DXVECTOR3( 1.0f, -1.0f,  0.0f), // Xyz
	D3DXVECTOR3(-1.0f,  1.0f,  0.0f), // xYz
	D3DXVECTOR3( 1.0f,  1.0f,  0.0f), // XYz
	D3DXVECTOR3(-1.0f, -1.0f,  1.0f), // xyZ
	D3DXVECTOR3( 1.0f, -1.0f,  1.0f), // XyZ
	D3DXVECTOR3(-1.0f,  1.0f,  1.0f), // xYZ
	D3DXVECTOR3( 1.0f,  1.0f,  1.0f), // XYZ
};

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide
{
	FS_NEAR		= 0,
	FS_FAR		= 1,
	FS_LEFT		= 2,
	FS_RIGHT	= 3,
	FS_TOP		= 4,
	FS_BOTTOM	= 5
}; 

//------------------------------------------------------------
void CDDFrustum::CalculateFrustum( D3DXMATRIX* modl , D3DXMATRIX* proj )
{
	D3DXMATRIXA16 mat;

	D3DXMatrixMultiply( &mat, modl, proj );
	D3DXMatrixInverse( &mat, NULL, &mat );

	// Corner
	D3DXVECTOR3	vecFrustum[8];

	for( INT i = 0; i < 8; i++ )
	{
		vecFrustum[i] = g_StandFrustum[i];
		D3DXVec3TransformCoord( &vecFrustum[i], &vecFrustum[i], &mat );
	}

	D3DXPlaneFromPoints( &m_Frustum[FS_NEAR], &vecFrustum[0], &vecFrustum[1], &vecFrustum[2] ); // Near
	D3DXPlaneFromPoints( &m_Frustum[FS_FAR], &vecFrustum[6], &vecFrustum[7], &vecFrustum[5] ); // Far
	D3DXPlaneFromPoints( &m_Frustum[FS_LEFT], &vecFrustum[2], &vecFrustum[6], &vecFrustum[4] ); // Left
	D3DXPlaneFromPoints( &m_Frustum[FS_RIGHT], &vecFrustum[7], &vecFrustum[3], &vecFrustum[5] ); // Right
	D3DXPlaneFromPoints( &m_Frustum[FS_TOP], &vecFrustum[2], &vecFrustum[3], &vecFrustum[6] ); // Top
	D3DXPlaneFromPoints( &m_Frustum[FS_BOTTOM], &vecFrustum[1], &vecFrustum[0], &vecFrustum[4] ); // Bottom


}

//------------------------------------------------------------
bool CDDFrustum::PointInFrustum( float x, float y, float z )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(m_Frustum[i].a * x + m_Frustum[i].b * y + m_Frustum[i].c * z + m_Frustum[i].d <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


//------------------------------------------------------------
bool CDDFrustum::SphereInFrustum( float x, float y, float z, float radius )
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )	
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( m_Frustum[i].a * x + m_Frustum[i].b * y + m_Frustum[i].c * z + m_Frustum[i].d <= -radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}

	// The sphere was inside of the frustum!
	return true;
}


//------------------------------------------------------------
bool CDDFrustum::CubeInFrustum( float x, float y, float z, float size )
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	for(int i = 0; i < 6; i++ )
	{
		if(m_Frustum[i].a * (x - size) + m_Frustum[i].b * (y - size) + m_Frustum[i].c * (z - size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x + size) + m_Frustum[i].b * (y - size) + m_Frustum[i].c * (z - size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x - size) + m_Frustum[i].b * (y + size) + m_Frustum[i].c * (z - size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x + size) + m_Frustum[i].b * (y + size) + m_Frustum[i].c * (z - size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x - size) + m_Frustum[i].b * (y - size) + m_Frustum[i].c * (z + size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x + size) + m_Frustum[i].b * (y - size) + m_Frustum[i].c * (z + size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x - size) + m_Frustum[i].b * (y + size) + m_Frustum[i].c * (z + size) + m_Frustum[i].d > 0)
			continue;
		if(m_Frustum[i].a * (x + size) + m_Frustum[i].b * (y + size) + m_Frustum[i].c * (z + size) + m_Frustum[i].d > 0)
			continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}


//------------------------------------------------------------
#define V_IN 1
#define V_OUT 2
#define V_INTERSECT 3

int CDDFrustum::BoxInFrustum( float x, float y, float z, float x2, float y2, float z2)
{
	// Detailed explanation on the BSP tutorial 4
	BYTE mode=0;					// set IN and OUT bit to 0
	for(int i = 0; i < 6; i++ )
	{
		mode &= V_OUT;				// clear the IN bit to 0 
		if( m_Frustum[i].a * x  + m_Frustum[i].b * y  + m_Frustum[i].c * z  + m_Frustum[i].d >= 0 )
			mode |= V_IN;  // set IN bit to 1
		else
			mode |= V_OUT;			// set OUT bit to 1

		// if we found a vertex IN for THIS plane and a vertex OUT of ANY plane continue
		// ( we have enough information to say: INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x2 + m_Frustum[i].b * y  + m_Frustum[i].c * z  + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x  + m_Frustum[i].b * y2 + m_Frustum[i].c * z  + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x2 + m_Frustum[i].b * y2 + m_Frustum[i].c * z  + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x  + m_Frustum[i].b * y  + m_Frustum[i].c * z2 + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x2 + m_Frustum[i].b * y  + m_Frustum[i].c * z2 + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x  + m_Frustum[i].b * y2 + m_Frustum[i].c * z2 + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		if(m_Frustum[i].a * x2 + m_Frustum[i].b * y2 + m_Frustum[i].c * z2 + m_Frustum[i].d >= 0)
			mode |= V_IN;
		else
			mode |= V_OUT;
		if (mode == V_INTERSECT) continue;

		// if we arrive to this point, then there are two possibilities:
		// there is not vertices in or there is not intersection till know, if 
		// there is a vertice in, continue (we are not over!) 
		if (mode == V_IN) continue;

		// there is not vertex IN front of this plane, so the box is COMPLETE_OUT
		return COMPLETE_OUT;
	}

	// All planes has a vertex IN FRONT so or the box is intersecting or complete IN
	if (mode == V_INTERSECT)
		return INTERSECT;
	else
		return COMPLETE_IN;
}