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
	
	// Ҷ��,tile���
	UINT leaf;

	// ��Χ��
	float minX , minY , minZ;
	float maxX , maxY , maxZ;
};

// ��Interlocking TilesΪLOD�㷨�ĵ���ʵ��
class CDDITTerrain
{
public:
	CDDITTerrain();
	~CDDITTerrain();

public:
	// ��������
	bool Initialize( const char* IniFile );

	// ���ٳ���
	void Destroy();

	// ���µ��� 
	void UpdateFrame();

	// ��Ⱦ���� 
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

	/** ��ȡ���ӷ�Χ */
	inline	float GetVisDistance() { 
		return m_fVisDistance; 
	}
public:

	bool IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& End , D3DXVECTOR3& CollidePoint );
	bool IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& dir , D3DXVECTOR3& CollidePoint , float& distance );

private:
	/** ���ݶ��������ɶ���ʵ�ʵ�XZֵ */
	inline float GenerateXPos( int idx ) { 
		return idx * m_fScalePlane; 
	}
	inline float GenerateZPos( int idx ) { 
		return idx * m_fScalePlane; 
	}
	/** ��ȡ�߶�ͼ
	 *	ע�����m_un16TileCountXZ�ڴ�֮ǰ�Ѿ���ֵ���������Ѿ�������m_un16TileCountXZ������ͨ��HMap�Լ����������ɡ�
	 *	���磬512 X 512��HMap������Tile9������TileΪ64 X 64��512 /(9��1)��
	 */
	bool LoadHeightMap( const char* map );
	void ReadHeightDatas( CDDImage& img );

	/** ��ȡ�߶�LOD����ͼ
	 *	@todo�����û�о������µģ�Ĭ�������Ǹ߶�ͼ������.hlod
	 */
	void LoadHLODMap( const char* map = "" );

	/** �����Ĳ��� */
	void GenerateQTree( QTreeNode* pCurNode , USHORT mintilex , USHORT mintiley , USHORT maxtilex , USHORT maxtiley );

	/** ��ȡ��ͼ���� */
	void LoadMapTexture( const char* texture );
	/** ��ȡϸ������ */
	void LoadDetailTexture( const char* texture );

private:
	/** �Ĳ�����׶�ü��������ɼ�Tile���� */
	void FrustumCull( QTreeNode* pCurNode , bool CheckFrustum = true );
	/** ����LOD������ֻ����ɼ�Tile��LOD */
	void GenerateLOD();
	/** ��Ⱦĳһ������ */
	void RenderTile( USHORT tilex , USHORT tiley );

private:
	/** X��Z�����Tile����
	 *
	 *	ͨ��Tile������ TILE_SIDE_VERT �Ϳ������������X��Z�����ϵĶ�������������Ҫ�����Ĳ���������Tile����������2�ı���
	 *	��������Ĳ���2�ı�������ô���Զ��������ڸ���ֵ����С��2�ı������������33����ѡ����64��
	 *
	 *	���Tile������ TILE_SIDE_VERT ��������Ķ������������ڸ߶�ͼ�Ķ���������
	 *	���������Ը߶�ͼ�������Բ�ֵ�����С����Ը߶�ͼ���е��Բ�ֵ��
	 *
	 *	���û���ڶ��߶�ͼǰ�������ֵ����ô�ͻ�ʹ�ø߶�ͼ�Ĵ�С�������ֵ��������ʹ�ø߶�ͼ�Ĵ�С���ԣ�TILE_SIDE_VERT-1����
	 *
	 */
	USHORT m_un16TileCountXZ;

	/** X����Ķ���������������Զ��������ֵ������Ҫ�ֶ��ύ */
	UINT				m_un32SideVertCount;
	/** ���ӵĺ���Scaleֵ */
	float				m_fScalePlane;
	/** ���ӵ�����Scaleֵ */
	float				m_fScaleHeight;

	/** һ��ϸ�������Ծ���ٸ����� */
	UCHAR				m_un8DetailScale;

	/** ����ͼ����Ұ��Χ��������ʱ��̬��� */
	float				m_fVisDistance;
	/** LOD����ֵ
	*	���õ�ʱ��ȡֵ��Χ�Ǵ�0.0��1.0 ��Ȼ�󽫸�����Ұ��Χ��̬���㵽���ʵ�ֵ
	*	����Ұ��Χ�仯��ʱ�����ֵҲ��ı�
	*/
	float				m_fLODRef[3];

	/** ����߶���Ϣ */
	float*				m_pfHMapInfos;

	/** Tile���� */
	_ITTile*			m_pTiles;

	/** ��պ� */
	CSkyBox*			m_pSkyBox;

private:
	/** �Ĳ����� */
	QTreeNode*			m_pQTreeRoot;

	/** �ɼ�Tile���� */
	typedef	list<UINT>	UINTList;
	UINTList			m_VisTileList;
private:
	// ������
	CDDTexture *m_pMainTexture;
	CDDTexture *m_pDetailTexture;
};


#endif