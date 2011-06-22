#ifndef _Prerequisites_H_
#define	_Prerequisites_H_


//////////////////////////////////////////////////////////////////////////
// cppstd
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <hash_set>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <queue>

#include <limits>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>


using namespace std;

//////////////////////////////////////////////////////////////////////////
// windows platform 
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// load the libs 
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxerr.lib")
#pragma comment(lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////////
// ������һЩ�޹ؽ�Ҫ��Warning���ص�����
#pragma warning( disable : 4251 )

// UINT��unsigned int������ �Ĳ�����ֵ������ʹ��UINT����������ô�ܿ����в����������
#define	INVALID_UINT_VALUE (unsigned int(-1))
// ����������Define��������Delete��Release��ʱ��ʹ���⼸������Define
// ���磬��ȥ�õ���
// int* p = new int;
// delete p;
// ������
// int* p = new int;
// SafeDelete(p);
// ������������˻�����ͬ���ֱ������ͷ�Com�ӿڣ�SafeRelease����ɾ�����飨SafeDeleteArray��
#define	SafeRelease(p) if(p)	{ p->Release() ;p = NULL; }
#define	SafeDelete(p) if(p)		{ delete p ;	p = NULL; }
#define	SafeDeleteArray(p) if(p){ delete[] p ;	p = NULL; }

//////////////////////////////////////////////////////////////////////////
// һЩ����ʵ�õ�Type define
typedef	vector<string>		StringVector;
typedef	vector<D3DXVECTOR3>	Vec3Vector;
typedef	vector<float>		FloatVector;
typedef	vector<int>			IntVector;
//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------
// pre defines �������ǰ������
class CDevice;
class CBillBoard;
class CCamera;
class CDDTexture;
class CSkyBox;
class CWOWWater;
class CDDITTerrain;
class CDDVertexBuffer;
class CDDIndexBuffer;
class CDDImage;
class CDDFrustum;
class CQuake3Level;

class CMeshBase;
class CMD2FrameMesh;
class CMdlSkinMesh;

class CParticle;
class CParticleAffector;
class CParticleEmitter;
class CParticleSystem;

class ConfigNode;



#endif//_Prerequisites_H_