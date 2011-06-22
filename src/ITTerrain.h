#ifndef __INTERLOCKTILES_TERRRAIN_H__
#define __INTERLOCKTILES_TERRRAIN_H__

#include "prerequisites.h"
#include "vertexindexbuffer.h"

#define	TILE_CENTER_VERT ((TILE_SIDE_VERT*TILE_SIDE_VERT - 1)/2)

struct _ITTile
{
	// VertexBuffer 
	CDDVertexBuffer m_vb;
	// Center Vector
	D3DXVECTOR3 m_vecCenter;
	// level of detail
	UCHAR m_lod;
};

struct TerrainVert
{
	enum
	{ 
		fvf = D3DFVF_XYZ | D3DFVF_TEX2 
	};
	D3DVECTOR pos;
	float u1,v1;
	float u2,v2;
	//float nx,ny,nz;
};

struct QTreeNode
{
	QTreeNode() : leaf(INVALID_UINT_VALUE) {
		children[0] = children[1] = children[2] = children[3] = NULL;
	}
	~QTreeNode() {
		for (int i=0;i<4;i++)
			SafeDelete(children[i]);
	}
	// Nodes
	QTreeNode *children[4];
	
	// 叶子,tile编号
	UINT leaf;

	// 包围盒
	float minX , minY , minZ;
	float maxX , maxY , maxZ;
};

// 用Interlocking Tiles为LOD算法的地形实现
class CDDITTerrain
{
public:
	CDDITTerrain();
	~CDDITTerrain();

public:
	// 建立场景
	bool Initialize( const char* IniFile );

	// 销毁场景
	void Destroy();

	// 更新地形 
	void UpdateFrame();

	// 渲染地形 
	void Render();

public:
	float GetHeight( float x , float z );
	inline float GetHeightByVert( UINT x , UINT z ) {
		if( (int)x >= (m_un32SideVertCount - 1) || (int)z >= (m_un32SideVertCount - 1) )	return 0.0f;
		return m_pfHMapInfos[z * m_un32SideVertCount + x];
	}

	inline float GetTotalWidth() { 
		return m_un32SideVertCount * m_fScalePlane; 
	}

	/** 获取可视范围 */
	inline	float GetVisDistance() { 
		return m_fVisDistance; 
	}
public:

	bool IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& End , D3DXVECTOR3& CollidePoint );
	bool IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& dir , D3DXVECTOR3& CollidePoint , float& distance );

private:
	/** 根据顶点编号生成顶点实际的XZ值 */
	inline float GenerateXPos( int idx ) { 
		return idx * m_fScalePlane; 
	}
	inline float GenerateZPos( int idx ) { 
		return idx * m_fScalePlane; 
	}
	/** 读取高度图
	 *	注：如果m_un16TileCountXZ在此之前已经赋值，则置信已经赋过的m_un16TileCountXZ，否则通过HMap自己的数据生成。
	 *	比如，512 X 512的HMap，对于Tile9，生成Tile为64 X 64（512 /(9－1)）
	 */
	bool LoadHeightMap( const char* map );
	void ReadHeightDatas( CDDImage& img );

	/** 读取高度LOD缓存图
	 *	@todo：如果没有就生成新的，默认名字是高度图本名＋.hlod
	 */
	void LoadHLODMap( const char* map = "" );

	/** 建立四叉树 */
	void GenerateQTree( QTreeNode* pCurNode , USHORT mintilex , USHORT mintiley , USHORT maxtilex , USHORT maxtiley );

	/** 读取地图纹理 */
	void LoadMapTexture( const char* texture );
	/** 读取细节纹理 */
	void LoadDetailTexture( const char* texture );

private:
	/** 四叉树视锥裁减，将填充可见Tile队列 */
	void FrustumCull( QTreeNode* pCurNode , bool CheckFrustum = true );
	/** 重算LOD，这里只计算可见Tile的LOD */
	void GenerateLOD();
	/** 渲染某一个格子 */
	void RenderTile( USHORT tilex , USHORT tiley );

private:
	/** X、Z方向的Tile数量
	 *
	 *	通过Tile数量和 TILE_SIDE_VERT 就可以算出来整个X、Z方向上的顶点数量，由于要是用四叉树，所以Tile数量必须是2的倍数
	 *	如果给出的不是2的倍数，那么就自动趋近大于给定值的最小的2的倍数，比如给定33，则选择向64。
	 *
	 *	如果Tile数量和 TILE_SIDE_VERT 计算出来的顶点数量不等于高度图的顶点数量，
	 *	如果大于则对高度图进行线性插值，如果小于则对高度图进行点性插值。
	 *
	 *	如果没有在读高度图前给出这个值，那么就会使用高度图的大小来算这个值，方法是使用高度图的大小除以（TILE_SIDE_VERT-1）。
	 *
	 */
	USHORT m_un16TileCountXZ;

	/** X方向的顶点数量，这个是自动算出来的值，不需要手动提交 */
	UINT				m_un32SideVertCount;
	/** 格子的横向Scale值 */
	float				m_fScalePlane;
	/** 格子的竖向Scale值 */
	float				m_fScaleHeight;

	/** 一幅细节纹理跨跃多少个格子 */
	UCHAR				m_un8DetailScale;

	/** 本地图的视野范围，可运行时动态变更 */
	float				m_fVisDistance;
	/** LOD的阈值
	*	设置的时候取值范围是从0.0到1.0 ，然后将根据视野范围动态计算到合适的值
	*	当视野范围变化的时候，这个值也会改变
	*/
	float				m_fLODRef[3];

	/** 顶点高度信息 */
	float*				m_pfHMapInfos;

	/** Tile数组 */
	_ITTile*			m_pTiles;

	/** 天空盒 */
	CSkyBox*			m_pSkyBox;

private:
	/** 四叉树根 */
	QTreeNode*			m_pQTreeRoot;

	/** 可见Tile队列 */
	typedef	list<UINT>	UINTList;
	UINTList			m_VisTileList;
private:
	// 主纹理
	CDDTexture *m_pMainTexture;
	CDDTexture *m_pDetailTexture;
};


#endif