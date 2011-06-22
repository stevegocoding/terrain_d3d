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
// 编译器一些无关紧要的Warning，关掉它。
#pragma warning( disable : 4251 )

// UINT（unsigned int）类型 的不合理值，例如使用UINT做索引，那么总可能有不合理的索引
#define	INVALID_UINT_VALUE (unsigned int(-1))
// 几个辅助的Define，建议在Delete、Release的时候使用这几个辅助Define
// 例如，过去用的是
// int* p = new int;
// delete p;
// 现在是
// int* p = new int;
// SafeDelete(p);
// 其他的两个与此基本相同，分别用于释放Com接口（SafeRelease）和删除数组（SafeDeleteArray）
#define	SafeRelease(p) if(p)	{ p->Release() ;p = NULL; }
#define	SafeDelete(p) if(p)		{ delete p ;	p = NULL; }
#define	SafeDeleteArray(p) if(p){ delete[] p ;	p = NULL; }

//////////////////////////////////////////////////////////////////////////
// 一些方便实用的Type define
typedef	vector<string>		StringVector;
typedef	vector<D3DXVECTOR3>	Vec3Vector;
typedef	vector<float>		FloatVector;
typedef	vector<int>			IntVector;
//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------
// pre defines 所有类的前置声明
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