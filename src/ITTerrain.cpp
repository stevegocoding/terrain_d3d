#include "ITTerrain.h"
#include "RenderDevice.h"
#include "Image.h"
#include "Texture.h"
#include "ITQTreeTile9X9IndexTable.h"
#include "Camera.h"
#include "Frustum.h"

#define	MAX_SIDE_VERT	4097

#define	QT_LEFT_TOP		0
#define	QT_RIGHT_TOP	1
#define	QT_LEFT_BOTTOM	2
#define	QT_RIGHT_BOTTOM	3

CDDITTerrain::CDDITTerrain()
{
	m_pfHMapInfos = NULL;
	m_pTiles = NULL;
	m_pQTreeRoot = NULL;
	m_pSkyBox = NULL;
	m_pMainTexture = NULL;
	m_pDetailTexture = NULL;
	m_VisTileList.clear();

}

CDDITTerrain::~CDDITTerrain()
{
	Destroy();
}

bool CDDITTerrain::Initialize( const char* IniFile )
{
	const char *heightMapName = "terrain_1_hmap.bmp";
	const char *texFileName = "terrain_1_tex.bmp";
	const char *detailTexFileName = "detail.bmp";

	g_IndexTable.initialize();

	// ����ϴε�
	Destroy();

	//ConfigNode cfg;
	//if( !ReadSimple( IniFile , cfg ) )
	//{ return false; }
	//ConfigNode* pCfg = &cfg;

	// Ԥ��ֵ
	m_fScalePlane = 4.0f;
	m_fScaleHeight = 4.0f;
	m_un16TileCountXZ = 0;	// 0��ʾʹ��HMap����ֵ�Զ�����
	m_un8DetailScale = 4;
	m_fVisDistance = 10000.0f;
	m_un32SideVertCount = 0;
	m_fLODRef[0] = 0.22f;
	m_fLODRef[1] = 0.44f;
	m_fLODRef[2] = 0.66f;

	/*
	// ������
	string map , lodmap , tex1 , tex2 , xzscale , yscale , detailtiles , sight , xztilenum , lod1 , lod2 , lod3 , sky;
	pCfg->GetAttribute( "�߶�ͼ" , map );
	pCfg->GetAttribute( "LOD����ͼ" , lodmap );
	pCfg->GetAttribute( "��պ�����·��" , sky );
	pCfg->GetAttribute( "������" , tex1 );
	pCfg->GetAttribute( "ϸ������" , tex2 );
	pCfg->GetAttribute( "ƽ����������" , xzscale );
	pCfg->GetAttribute( "�߶���������" , yscale );
	pCfg->GetAttribute( "ϸ��������" , detailtiles );
	pCfg->GetAttribute( "��Ұ��Χ" , sight );
	pCfg->GetAttribute( "ƽ��Tile����" , xztilenum );
	pCfg->GetAttribute( "LOD����3��ֵ" , lod3 );
	pCfg->GetAttribute( "LOD����2��ֵ" , lod2 );
	pCfg->GetAttribute( "LOD����1��ֵ" , lod1 );

	// �����Ҫ���ģ��͸���Ԥ��ֵ
	if ( xzscale != "" )	{ m_fScalePlane = atof( xzscale.c_str() ); }
	if ( yscale != "" )		{ m_fScaleHeight = atof( yscale.c_str() ); }
	if ( xztilenum != "" )	{ m_un16TileCountXZ = atoi( xztilenum.c_str() ); }
	if ( detailtiles != "" ){ m_un8DetailScale = atoi( detailtiles.c_str() ); }
	if ( sight != "" )		{ m_fVisDistance = atof( sight.c_str() ); }
	if ( lod3 != "" )		{ m_fLODRef[0] = atof( lod3.c_str() ); }
	if ( lod2 != "" )		{ m_fLODRef[1] = atof( lod2.c_str() ); }
	if ( lod1 != "" )		{ m_fLODRef[2] = atof( lod1.c_str() ); }
	*/
	m_fLODRef[0] *= m_fVisDistance;
	m_fLODRef[1] *= m_fVisDistance;
	m_fLODRef[2] *= m_fVisDistance;

	GetRenderDevice().GetCurrentCamera()->SetFarClip( m_fVisDistance );

	if( !LoadHeightMap( heightMapName ) ) { 
		return false; 
	}

	
	// LoadHLODMap( lodmap.c_str() );
	LoadMapTexture( texFileName );
	LoadDetailTexture( detailTexFileName );
	
	m_pQTreeRoot = new QTreeNode;
	GenerateQTree( m_pQTreeRoot , 0 , 0 , m_un16TileCountXZ - 1 , m_un16TileCountXZ - 1 );

	return true;
}

void CDDITTerrain::Destroy()
{
	SafeDelete(	m_pMainTexture );
	SafeDelete(	m_pDetailTexture );
	SafeDelete( m_pQTreeRoot );
	SafeDeleteArray( m_pfHMapInfos );
	SafeDeleteArray( m_pTiles );
}

void CDDITTerrain::UpdateFrame()
{
	// ��ն���
	m_VisTileList.clear();

	// ��׶�ü�
	FrustumCull( m_pQTreeRoot );

	// ����LOD
	GenerateLOD();
}

void CDDITTerrain::Render()
{
	if ( !m_pTiles )
	{ return ; }

	// ����о�ʹ��ȫ����
	GetRenderDevice().UseGlobalFog(true);

	GetRenderDevice().SetDepthEnabled( true , true );

	// ���ڳ˷��ľ���
	D3DXMATRIXA16 tmp;
	D3DXMatrixIdentity( &tmp );
	GetRenderDevice().SetWorldMatrix( tmp );

	// ����Index���ֵĹ�ϵ������ʹ�ö�˳ʱ�������βü�
	GetRenderDevice().GetD3DDevice()->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW );

	// ��������D3DTEXF_ANISOTROPIC�����������Щ������Linear���� ^_^
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 0 , D3DSAMP_MIPFILTER , D3DTEXF_LINEAR );
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 1 , D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 1 , D3DSAMP_MINFILTER , D3DTEXF_LINEAR );
	GetRenderDevice().GetD3DDevice()->SetSamplerState( 1 , D3DSAMP_MIPFILTER , D3DTEXF_LINEAR );

	GetRenderDevice().SetTextureColorOp( 0 , D3DTOP_SELECTARG1 , D3DTA_TEXTURE , D3DTA_DIFFUSE );
	GetRenderDevice().SetTextureColorOp( 1 , D3DTOP_MODULATE2X	, D3DTA_TEXTURE , D3DTA_CURRENT );
	GetRenderDevice().SetTexture( 0 , *m_pMainTexture );
	GetRenderDevice().SetTexture( 1 , *m_pDetailTexture );

	UINTList::iterator it = m_VisTileList.begin();
	for ( ; it != m_VisTileList.end() ; ++it )
	{
		UINT y = (*it) / m_un16TileCountXZ;
		UINT x = (*it) % m_un16TileCountXZ;
		RenderTile( x , y );
	}
	// ���β���Ⱦ����
	GetRenderDevice().GetD3DDevice()->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
}

bool CDDITTerrain::LoadHeightMap( const char* map )
{
	CDDImage img;
	if( !img.loadBmp( map ) )
	{ return false; }

	//// ������ǻҶ�ͼ������
	//if ( img.GetBytePerPixel() != 1 )
	//{ return false; }
	// ��������ȣ�����
	if ( img.getWidth() != img.getHeight() )
	{ return false; }

	// ���Ƿ���Ҫ�仯m_un16TileCountXZ���������ڴ�֮ǰ�Ѿ���ֵ�Ͳ����ٴ󶯸ɸ꣬�������û�У��ͱ����Զ���һ�������ֵ
	if ( !m_un16TileCountXZ )
	{ m_un16TileCountXZ = img.getWidth() / ( TILE_SIDE_VERT - 1 ); }

	//// m_un16TileCountXZ������һ��2��n�η���ֵ���˴���������ԭ�򣬸��Ĳ���ȷ��ֵ
	//_Normalize2NGreat( m_un16TileCountXZ );

	// ���ֵ̫���ˣ�����
	m_un32SideVertCount = m_un16TileCountXZ * ( TILE_SIDE_VERT - 1 ) + 1;
	if ( m_un32SideVertCount > MAX_SIDE_VERT )
	{ return false; }

	// ���ڿ���ȷ��m_un16TileCountXZ��һ����ȷֵ�ˣ���ʼ����߶����ݡ�
	ReadHeightDatas( img );

	// �߶�����׼����ϣ��������ɶ�����Ϣ��
	m_pTiles = new _ITTile[ m_un16TileCountXZ * m_un16TileCountXZ ];
	TerrainVert* pTmp = new TerrainVert[ TILE_SIDE_VERT * TILE_SIDE_VERT ];

	// ������
	float	texfactor = 1.0f / (m_un32SideVertCount - 1);
	// ϸ������
	float	tex2factor = texfactor * ( m_un16TileCountXZ / m_un8DetailScale );

	// ��ʼ���������
	UINT xstart , ystart , xend , yend ;
	for ( int i = 0 ; i < m_un16TileCountXZ ; ++i )
	{
		for ( int j = 0 ; j < m_un16TileCountXZ ; ++j )
		{
			xstart = j * ( TILE_SIDE_VERT - 1 );
			ystart = i * ( TILE_SIDE_VERT - 1 );
			xend = xstart + TILE_SIDE_VERT;
			yend = ystart + TILE_SIDE_VERT;

			// ���ĳ��Tile��ֵ
			for ( UINT y = ystart ; y < yend ; y++ )
			{
				for ( UINT x = xstart ; x < xend ; x++ )
				{
					UINT tilex = x - xstart;
					UINT tiley = y - ystart;

#define TMPIDX (tiley * TILE_SIDE_VERT + tilex)

					pTmp[TMPIDX].pos.x = GenerateXPos( x );
					pTmp[TMPIDX].pos.z = GenerateZPos( y );
					pTmp[TMPIDX].pos.y = m_pfHMapInfos[ y * m_un32SideVertCount + x ];
					pTmp[TMPIDX].u1 = texfactor * x;
					pTmp[TMPIDX].v1 = texfactor * y;
					pTmp[TMPIDX].u2 = tex2factor * x;
					pTmp[TMPIDX].v2 = tex2factor * y;

					if ( TMPIDX == TILE_CENTER_VERT )
					{ m_pTiles[ i*m_un16TileCountXZ + j ].m_vecCenter = pTmp[TMPIDX].pos; }
				}

#undef TMPIDX

			}
			// ������Tile��VB
			m_pTiles[ i*m_un16TileCountXZ + j ].m_vb.createFVF( TerrainVert::fvf , TILE_SIDE_VERT*TILE_SIDE_VERT , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
			m_pTiles[ i*m_un16TileCountXZ + j ].m_vb.writeData( 0 , TILE_SIDE_VERT*TILE_SIDE_VERT*sizeof(TerrainVert) , pTmp );
		}
	}

	SafeDeleteArray(pTmp);

	return true;
}

void CDDITTerrain::ReadHeightDatas(CDDImage &img)
{
	// ���ڶ���������������һ��ȫ�ȣ����ԣ�����Ը߶�ͼ�����ݽ��в�ֵ

	// ����߶�ͼ��С��510 X 510����ô����TileCountӦ�ò��� 510/(9-1) = 63 �� ����64��Ҳ����һ��513�����㣬510��513����Щ�����Ա�����в�ֵ
	// �ȿ����Ƿ���Ҫ��ֵ�������������͸߶�ͼ�������������ϵ�ʱ�򣬾ͱ����ֵ

	// ���㲻����ˣ���ôһ������ϰ�ߴ�������512 X 512�ĸ߶�ͼҲ����Ӧ513 X 513�Ķ���ͼ���Ծ���Ҫ��ֵ
	// ֻ�е���ȫȫ�ȵ�ʱ�򣬲Ų���Ҫ��ֵ����������ǽ�ȫ�ȿ���һ�������������ô�������Ǿ͸��Ӽ���
	// ���ڲ�ֵ������һ�����裬���ǵ�һ�����Ǿ�����ȫ���е�һ�����أ����һ����Ҳ�Ǿ�����ȫ�������һ������

	// ��ֵ����������������ֵ����С��ֵ��Ŀǰ����ʵ�ֵ��㷨�������ֵʹ��������ֵ����С��ֵʹ��������ֵ

	// ��ֵ���ӣ��ٸ����ӣ�����ĵ�һ����������ͼ�ϵ�������0X��ֵ���ӣ��ڶ�����1X��ֵ���ӣ��������ƣ�ֱ�����һ����λ��������img�ĺ�����
	float factor = ((float) (img.getWidth() - 1)) / ((float) ( m_un32SideVertCount  - 1 ) );

	// ���ɸ߶���Ϣ
	m_pfHMapInfos = new float[ m_un32SideVertCount * m_un32SideVertCount ];

	USHORT px , py;
	for ( UINT i = 0 ; i < m_un32SideVertCount ; ++i )
	{
		for ( UINT j = 0 ; j < m_un32SideVertCount ; ++j )
		{
			// �ҵ��������������ͼ�ϵ�����λ��
			px = j * factor;
			py = i * factor;

			// ȡ�����صĸ߶�
			m_pfHMapInfos[ m_un32SideVertCount * i + j ] = img.getColor8( px , py ) * m_fScaleHeight;
		}
	}
}

void CDDITTerrain::GenerateQTree(QTreeNode* pCurNode , USHORT mintilex , USHORT mintiley , USHORT maxtilex , USHORT maxtiley)
{
	// ���ɱ��ڵİ�Χ��
	// ��ʼ�����ֹ������(����������)
	UINT xstart = mintilex * (TILE_SIDE_VERT - 1);
	UINT ystart = mintiley * (TILE_SIDE_VERT - 1);
	UINT xend = (maxtilex + 1) * ( TILE_SIDE_VERT - 1);
	UINT yend = (maxtiley + 1) * ( TILE_SIDE_VERT - 1);
	// �����������С��ʵ�ʶ���xzλ��(��Χ��)
	pCurNode->minX = GenerateXPos(xstart);
	pCurNode->minZ = GenerateZPos(ystart);
	pCurNode->maxX = GenerateXPos(xend);
	pCurNode->maxZ = GenerateZPos(yend);
	
	// ����������С�ĸ߶�ֵ
	pCurNode->minY = 0;
	pCurNode->maxY = 255 * m_fScaleHeight;

	// ����ֵ���ϸ�ˣ�������һ��Ҷ��
	if ( ( mintilex == maxtilex ) && ( mintiley == maxtiley ) )
	{
		// leaf �� tile �������е�����
		pCurNode->leaf = mintiley * m_un16TileCountXZ + mintilex;
		return;
	}

	// X��Y�Ķ��ֵ�
	USHORT midx = mintilex + ( ( maxtilex - mintilex ) >> 1 );
	USHORT midy = mintiley + ( ( maxtiley - mintiley ) >> 1 );

	// ����
	QTreeNode* pNode = new QTreeNode;
	pCurNode->children[QT_LEFT_TOP] = pNode;
	GenerateQTree( pNode , mintilex , mintiley , midx , midy );
	// ����
	pNode = new QTreeNode;
	pCurNode->children[QT_RIGHT_TOP] = pNode;
	GenerateQTree( pNode , midx + 1 , mintiley , maxtilex , midy );
	// ����
	pNode = new QTreeNode;
	pCurNode->children[QT_LEFT_BOTTOM] = pNode;
	GenerateQTree( pNode , mintilex , midy + 1 , midx , maxtiley );
	// ����
	pNode = new QTreeNode;
	pCurNode->children[QT_RIGHT_BOTTOM] = pNode;
	GenerateQTree( pNode , midx + 1 , midy + 1 , maxtilex , maxtiley );
}

void CDDITTerrain::LoadMapTexture(const char *texture)
{
	m_pMainTexture = new CDDTexture;
	m_pMainTexture->createFromFile(texture);
}

void CDDITTerrain::LoadDetailTexture(const char *texture)
{
	m_pDetailTexture = new CDDTexture;
	m_pDetailTexture->createFromFile(texture);
}

void CDDITTerrain::FrustumCull( QTreeNode* pCurNode , bool CheckFrustum /*= true*/ )
{
	if ( !pCurNode ) {
		assert(false);
		return;
	}

	// ����ڵ�İ�Χ�к�frustum�Ĺ�ϵ
	if ( CheckFrustum ) {
		switch( GetRenderDevice().GetCurrentCamera()->GetFrustum()->
			BoxInFrustum( pCurNode->minX , pCurNode->minY , pCurNode->minZ, pCurNode->maxX , pCurNode->maxY , pCurNode->maxZ ) )
		{
			// ȫ����Ҳ���ؿ��ˣ�����Ҷ�ӿ϶�Ҳ������
		case COMPLETE_OUT:
			return;
			// �����ǰNode��CompleteIn��׶�Ļ����Ͳ���Ҫ�����ü��ˣ�ȫ������׶�ڻ��ø�ͷ��
		case COMPLETE_IN:
			CheckFrustum = false;
		}
	}

	// �����Ҷ�ӣ��Ͱ�Ҷ�ӷ���ɼ�����
	if ( pCurNode->leaf != INVALID_UINT_VALUE ) {
		m_VisTileList.push_back(pCurNode->leaf);
		return;
	}

	// ����ӽڵ�
	for ( UCHAR i = 0 ; i < 4 ; i++ ) { 
		FrustumCull( pCurNode->children[i] , CheckFrustum ); 
	}
}

void CDDITTerrain::GenerateLOD()
{
	D3DXVECTOR3	campos = *(GetRenderDevice().GetCurrentCamera()->GetPosition());

	// �������пɼ���Tile������Tile���ӵ��λ�ü���LOD�����ļ�
	UINTList::iterator it = m_VisTileList.begin();
	for ( ; it != m_VisTileList.end() ; ++it )
	{
		D3DXVECTOR3 VectorToCamera( m_pTiles[*it].m_vecCenter - campos );
		float DistanceToCamera = D3DXVec3Length( &VectorToCamera );

		if ( DistanceToCamera < m_fLODRef[0] )
		{ m_pTiles[*it].m_lod = 3; }
		else if ( DistanceToCamera < m_fLODRef[1] )
		{ m_pTiles[*it].m_lod = 2; }
		else if ( DistanceToCamera < m_fLODRef[2] )
		{ m_pTiles[*it].m_lod = 1; }
		else
		{ m_pTiles[*it].m_lod = 0; }
	}
}

void CDDITTerrain::RenderTile( USHORT tilex , USHORT tiley )
{
	// top, left, right, bottom
	bool t = false , l = false , r = false , b = false;

	UCHAR thislevel = m_pTiles[ tiley*m_un16TileCountXZ+tilex].m_lod;
	CDDVertexBuffer* thisvb = &(m_pTiles[ tiley*m_un16TileCountXZ+tilex].m_vb);
	UCHAR tlv = 0 , llv = 0 , rlv = 0 , blv = 0;

	// ��
	if ( tiley )
	{
		// �õ���ǰtile�����Ǹ�tile��lod����
		tlv = m_pTiles[( tiley-1 )*m_un16TileCountXZ+tilex ].m_lod;
		// ��������Ǹ�tile��lodС�ڵ�ǰ���tile(�ֲ�)
		if ( tlv < thislevel )
		{
			// Set the flag
			t = true;
			CDDIndexBuffer* ib = g_IndexTable.getConnectIB( thislevel , TOP , tlv );
			GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
		}
	}
	// ��
	if ( tiley < ( m_un16TileCountXZ-1 ) )
	{
		blv = m_pTiles[( tiley+1 )*m_un16TileCountXZ+tilex ].m_lod;
		if ( blv < thislevel )
		{
			b = true;
			CDDIndexBuffer* ib = g_IndexTable.getConnectIB( thislevel , BOTTOM , blv );
			GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
		}
	}
	// ��
	if ( tilex )
	{
		llv = m_pTiles[tiley*m_un16TileCountXZ+tilex-1].m_lod;
		if ( llv < thislevel )
		{
			l = true;
			CDDIndexBuffer* ib = g_IndexTable.getConnectIB( thislevel , LEFT , llv );
			GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
		}
	}
	// ��
	if ( tilex < ( m_un16TileCountXZ-1 ) )
	{
		rlv = m_pTiles[tiley*m_un16TileCountXZ+tilex+1 ].m_lod;
		if ( rlv < thislevel )
		{
			r = true;
			CDDIndexBuffer* ib = g_IndexTable.getConnectIB( thislevel , RIGHT , rlv );
			GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
		}
	}
	// ����
	CDDIndexBuffer* ib = g_IndexTable.getBodyIB( thislevel , t , l , r , b );
	if ( ib )
	{
		GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
	}
}

float CDDITTerrain::GetHeight( float x , float z )
{
	float tmp = 1.0f / m_fScalePlane;
	UINT subscript_x = x * tmp;
	UINT subscript_z = z * tmp;
	float x_delta = x * tmp - subscript_x;
	float z_delta = z * tmp - subscript_z;

	if( subscript_x >= (m_un32SideVertCount - 1) || subscript_z >= (m_un32SideVertCount - 1) )
		return false;

	// ��ȡ�ĸ�����ĸ߶�
	float h11 = m_pfHMapInfos[subscript_z * m_un32SideVertCount + subscript_x];
	float h12 = m_pfHMapInfos[subscript_z * m_un32SideVertCount + subscript_x+1];
	float h21 = m_pfHMapInfos[(subscript_z + 1) * m_un32SideVertCount + subscript_x];
	float h22 = m_pfHMapInfos[(subscript_z + 1) * m_un32SideVertCount + subscript_x +1];

	// ��ֵ
	float ha = h11 + x_delta * (h12 - h11);
	float hb = h21 + x_delta * (h22 - h21);

	return (ha + z_delta * (hb - ha));
}

bool CDDITTerrain::IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& End , D3DXVECTOR3& CollidePoint )
{
	float tmp = 1.0f / m_fScalePlane;
	// ���߷������������ʱ�յ㣬������ײ�ж�
	D3DXVECTOR3 vTempPos;

	D3DXVECTOR3 vStep = ( End - ori );
	// ���
	float range = D3DXVec3Length(&vStep) ;
	// ��λ����
	vStep = vStep/range;

	// һ������һ�����ӽ����ж�
	for( int i=0 ; i < range ; i += m_fScalePlane )
	{
		vTempPos = vStep * i + ori;
		// ȡ��ǰ����Զ�˵��Ӧ�ĵ��ζ��������±�
		UINT subscript_x = vTempPos.x * tmp;
		UINT subscript_z = vTempPos.z * tmp;

		if( subscript_x >= m_un32SideVertCount-1 || subscript_z >= m_un32SideVertCount-1 )
			return false;

		// �������Զ���YС�ڶ�Ӧ���ζ����Y����Ϊ�������������
		if( GetHeightByVert( subscript_x , subscript_z ) > vTempPos.y )
		{
			// ��ȷ��������º����߻���1�����淽��
			vTempPos -= vStep * m_fScalePlane;

			// ���˺���С����������ÿ��1����λ�ٴ��������߽�����ϸ�ж�
			for( int j = 0 ; j < m_fScalePlane * 2 ; j++)
			{
				vTempPos += vStep;

				// ȡ����ȷ����ײ�����߶ȣ��Ƕ��������ϵģ�
				float terrain_y = GetHeight( vTempPos.x , vTempPos.z );

				// �жϸ߶ȣ��Ƿ��Ѿ��������
				if( terrain_y >= vTempPos.y )
				{
					CollidePoint = vTempPos;
					CollidePoint.y = terrain_y;
					return true;
				}
			}
		}
	}
	return false;
}

bool CDDITTerrain::IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& dir  , D3DXVECTOR3& CollidePoint , float& distance)
{
	float tmp = 1.0f / m_fScalePlane;

	// ���߷������������ʱ�յ㣬������ײ�ж�
	D3DXVECTOR3 vTempPos;

	// dir ������ǵ�λʸ��������������λ����
	D3DXVECTOR3 vStep = dir;

	// һ������һ�����ӣ�Ĭ����32����λ�������ж�
	for( int i=0 ; i < distance ; i += m_fScalePlane )
	{
		vTempPos = vStep * i + ori;
		// ȡ��ǰ����Զ�˵��Ӧ�ĵ��ζ��������±�
		UINT subscript_x = vTempPos.x * tmp;
		UINT subscript_z = vTempPos.z * tmp;

		if( subscript_x >= m_un32SideVertCount-1 || subscript_z >= m_un32SideVertCount-1 )
			return false;

		// �������Զ���YС�ڶ�Ӧ���ζ����Y����Ϊ�������������
		if( GetHeightByVert( subscript_x , subscript_z ) > vTempPos.y )
		{
			// ��ȷ��������º����߻���1�����淽��
			vTempPos -= vStep * m_fScalePlane;

			// ���˺���С����������ÿ��1����λ�ٴ��������߽�����ϸ�ж�
			for( int j = 0 ; j < m_fScalePlane * 2 ; j++)
			{
				vTempPos += vStep;

				// ȡ����ȷ����ײ�����߶ȣ��Ƕ��������ϵģ�
				float terrain_y = GetHeight( vTempPos.x , vTempPos.z );

				// �жϸ߶ȣ��Ƿ��Ѿ��������
				if( terrain_y >= vTempPos.y )
				{
					CollidePoint = vTempPos;
					// �����ײ�㵽ԭʼ�������ߵĳ��ȣ���Ϊdistance����
					vTempPos -= ori;
					distance = D3DXVec3Length( &vTempPos );
					CollidePoint.y = terrain_y;
					return true;
				}
			}
		}
	}
	return false;
}