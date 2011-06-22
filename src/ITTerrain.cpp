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

	// 清空上次的
	Destroy();

	//ConfigNode cfg;
	//if( !ReadSimple( IniFile , cfg ) )
	//{ return false; }
	//ConfigNode* pCfg = &cfg;

	// 预设值
	m_fScalePlane = 4.0f;
	m_fScaleHeight = 4.0f;
	m_un16TileCountXZ = 0;	// 0表示使用HMap的数值自动生成
	m_un8DetailScale = 4;
	m_fVisDistance = 10000.0f;
	m_un32SideVertCount = 0;
	m_fLODRef[0] = 0.22f;
	m_fLODRef[1] = 0.44f;
	m_fLODRef[2] = 0.66f;

	/*
	// 读配置
	string map , lodmap , tex1 , tex2 , xzscale , yscale , detailtiles , sight , xztilenum , lod1 , lod2 , lod3 , sky;
	pCfg->GetAttribute( "高度图" , map );
	pCfg->GetAttribute( "LOD因子图" , lodmap );
	pCfg->GetAttribute( "天空盒纹理路径" , sky );
	pCfg->GetAttribute( "主纹理" , tex1 );
	pCfg->GetAttribute( "细节纹理" , tex2 );
	pCfg->GetAttribute( "平面缩放因子" , xzscale );
	pCfg->GetAttribute( "高度缩放因子" , yscale );
	pCfg->GetAttribute( "细节纹理跨度" , detailtiles );
	pCfg->GetAttribute( "视野范围" , sight );
	pCfg->GetAttribute( "平面Tile数量" , xztilenum );
	pCfg->GetAttribute( "LOD级别3阈值" , lod3 );
	pCfg->GetAttribute( "LOD级别2阈值" , lod2 );
	pCfg->GetAttribute( "LOD级别1阈值" , lod1 );

	// 如果需要更改，就更改预定值
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
	// 清空队列
	m_VisTileList.clear();

	// 视锥裁减
	FrustumCull( m_pQTreeRoot );

	// 生成LOD
	GenerateLOD();
}

void CDDITTerrain::Render()
{
	if ( !m_pTiles )
	{ return ; }

	// 如果有就使用全局雾
	GetRenderDevice().UseGlobalFog(true);

	GetRenderDevice().SetDepthEnabled( true , true );

	// 用于乘法的矩阵
	D3DXMATRIXA16 tmp;
	D3DXMatrixIdentity( &tmp );
	GetRenderDevice().SetWorldMatrix( tmp );

	// 由于Index布局的关系，这里使用对顺时针三角形裁减
	GetRenderDevice().GetD3DDevice()->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW );

	// 毫无疑问D3DTEXF_ANISOTROPIC可能质量会好些，但是Linear更快 ^_^
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
	// 地形不渲染背面
	GetRenderDevice().GetD3DDevice()->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
}

bool CDDITTerrain::LoadHeightMap( const char* map )
{
	CDDImage img;
	if( !img.loadBmp( map ) )
	{ return false; }

	//// 如果不是灰度图，错误
	//if ( img.GetBytePerPixel() != 1 )
	//{ return false; }
	// 如果长宽不等，错误
	if ( img.getWidth() != img.getHeight() )
	{ return false; }

	// 看是否需要变化m_un16TileCountXZ，如果这个在此之前已经赋值就不必再大动干戈，但是如果没有，就必须自动给一个合理的值
	if ( !m_un16TileCountXZ )
	{ m_un16TileCountXZ = img.getWidth() / ( TILE_SIDE_VERT - 1 ); }

	//// m_un16TileCountXZ必须是一个2的n次方数值，此处按照扩大原则，更改不正确的值
	//_Normalize2NGreat( m_un16TileCountXZ );

	// 如果值太大了，错误
	m_un32SideVertCount = m_un16TileCountXZ * ( TILE_SIDE_VERT - 1 ) + 1;
	if ( m_un32SideVertCount > MAX_SIDE_VERT )
	{ return false; }

	// 现在可以确定m_un16TileCountXZ是一个正确值了，开始读入高度数据。
	ReadHeightDatas( img );

	// 高度数据准备完毕，可以生成顶点信息了
	m_pTiles = new _ITTile[ m_un16TileCountXZ * m_un16TileCountXZ ];
	TerrainVert* pTmp = new TerrainVert[ TILE_SIDE_VERT * TILE_SIDE_VERT ];

	// 主纹理
	float	texfactor = 1.0f / (m_un32SideVertCount - 1);
	// 细节纹理
	float	tex2factor = texfactor * ( m_un16TileCountXZ / m_un8DetailScale );

	// 开始顶点的索引
	UINT xstart , ystart , xend , yend ;
	for ( int i = 0 ; i < m_un16TileCountXZ ; ++i )
	{
		for ( int j = 0 ; j < m_un16TileCountXZ ; ++j )
		{
			xstart = j * ( TILE_SIDE_VERT - 1 );
			ystart = i * ( TILE_SIDE_VERT - 1 );
			xend = xstart + TILE_SIDE_VERT;
			yend = ystart + TILE_SIDE_VERT;

			// 填充某个Tile的值
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
			// 填充这个Tile的VB
			m_pTiles[ i*m_un16TileCountXZ + j ].m_vb.createFVF( TerrainVert::fvf , TILE_SIDE_VERT*TILE_SIDE_VERT , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
			m_pTiles[ i*m_un16TileCountXZ + j ].m_vb.writeData( 0 , TILE_SIDE_VERT*TILE_SIDE_VERT*sizeof(TerrainVert) , pTmp );
		}
	}

	SafeDeleteArray(pTmp);

	return true;
}

void CDDITTerrain::ReadHeightDatas(CDDImage &img)
{
	// 由于顶点数和像素数不一定全等，所以，必须对高度图的数据进行插值

	// 例如高度图大小是510 X 510，那么现在TileCount应该不是 510/(9-1) = 63 ， 而是64，也就是一边513个顶点，510和513差了些，所以必须进行插值
	// 先看看是否需要插值，当顶点数量和高度图像素数量不符合的时候，就必须插值

	// 即便不是如此，那么一般人们习惯传进来的512 X 512的高度图也不适应513 X 513的顶点图，仍旧需要插值
	// 只有当完全全等的时候，才不需要插值，但如果我们将全等看作一种特殊情况，那么这里我们就更加简单了
	// 对于插值我们有一个假设，就是第一个点是绝对完全命中第一个像素，最后一个点也是绝对完全命中最后一个像素

	// 插值有两种情况，增大插值和缩小插值，目前我们实现的算法是增大差值使用最近点插值，缩小差值使用最近点插值

	// 插值因子，举个例子，横向的第一个点在像素图上的坐标是0X插值因子，第二个点1X插值因子，依此类推，直到最后一个点位置正好是img的横向宽度
	float factor = ((float) (img.getWidth() - 1)) / ((float) ( m_un32SideVertCount  - 1 ) );

	// 生成高度信息
	m_pfHMapInfos = new float[ m_un32SideVertCount * m_un32SideVertCount ];

	USHORT px , py;
	for ( UINT i = 0 ; i < m_un32SideVertCount ; ++i )
	{
		for ( UINT j = 0 ; j < m_un32SideVertCount ; ++j )
		{
			// 找到这个顶点在像素图上的像素位置
			px = j * factor;
			py = i * factor;

			// 取此像素的高度
			m_pfHMapInfos[ m_un32SideVertCount * i + j ] = img.getColor8( px , py ) * m_fScaleHeight;
		}
	}
}

void CDDITTerrain::GenerateQTree(QTreeNode* pCurNode , USHORT mintilex , USHORT mintiley , USHORT maxtilex , USHORT maxtiley)
{
	// 生成本节的包围盒
	// 起始点和终止点索引(顶点索引号)
	UINT xstart = mintilex * (TILE_SIDE_VERT - 1);
	UINT ystart = mintiley * (TILE_SIDE_VERT - 1);
	UINT xend = (maxtilex + 1) * ( TILE_SIDE_VERT - 1);
	UINT yend = (maxtiley + 1) * ( TILE_SIDE_VERT - 1);
	// 计算出最大和最小的实际顶点xz位置(包围盒)
	pCurNode->minX = GenerateXPos(xstart);
	pCurNode->minZ = GenerateZPos(ystart);
	pCurNode->maxX = GenerateXPos(xend);
	pCurNode->maxZ = GenerateZPos(yend);
	
	// 计算最大和最小的高度值
	pCurNode->minY = 0;
	pCurNode->maxY = 255 * m_fScaleHeight;

	// 如果分到最细了，就生成一个叶子
	if ( ( mintilex == maxtilex ) && ( mintiley == maxtiley ) )
	{
		// leaf 是 tile 在数组中的索引
		pCurNode->leaf = mintiley * m_un16TileCountXZ + mintilex;
		return;
	}

	// X和Y的二分点
	USHORT midx = mintilex + ( ( maxtilex - mintilex ) >> 1 );
	USHORT midy = mintiley + ( ( maxtiley - mintiley ) >> 1 );

	// 左上
	QTreeNode* pNode = new QTreeNode;
	pCurNode->children[QT_LEFT_TOP] = pNode;
	GenerateQTree( pNode , mintilex , mintiley , midx , midy );
	// 右上
	pNode = new QTreeNode;
	pCurNode->children[QT_RIGHT_TOP] = pNode;
	GenerateQTree( pNode , midx + 1 , mintiley , maxtilex , midy );
	// 左下
	pNode = new QTreeNode;
	pCurNode->children[QT_LEFT_BOTTOM] = pNode;
	GenerateQTree( pNode , mintilex , midy + 1 , midx , maxtiley );
	// 右下
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

	// 计算节点的包围盒和frustum的关系
	if ( CheckFrustum ) {
		switch( GetRenderDevice().GetCurrentCamera()->GetFrustum()->
			BoxInFrustum( pCurNode->minX , pCurNode->minY , pCurNode->minZ, pCurNode->maxX , pCurNode->maxY , pCurNode->maxZ ) )
		{
			// 全出，也不必看了，所有叶子肯定也都出了
		case COMPLETE_OUT:
			return;
			// 如果当前Node是CompleteIn视锥的话，就不需要继续裁减了，全都在视锥内还裁个头啊
		case COMPLETE_IN:
			CheckFrustum = false;
		}
	}

	// 如果是叶子，就把叶子放入可见队列
	if ( pCurNode->leaf != INVALID_UINT_VALUE ) {
		m_VisTileList.push_back(pCurNode->leaf);
		return;
	}

	// 检查子节点
	for ( UCHAR i = 0 ; i < 4 ; i++ ) { 
		FrustumCull( pCurNode->children[i] , CheckFrustum ); 
	}
}

void CDDITTerrain::GenerateLOD()
{
	D3DXVECTOR3	campos = *(GetRenderDevice().GetCurrentCamera()->GetPosition());

	// 遍历所有可见的Tile，按照Tile到视点的位置计算LOD，共四级
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

	// 上
	if ( tiley )
	{
		// 得到当前tile上面那个tile的lod级别
		tlv = m_pTiles[( tiley-1 )*m_un16TileCountXZ+tilex ].m_lod;
		// 如果上面那个tile的lod小于当前这个tile(粗糙)
		if ( tlv < thislevel )
		{
			// Set the flag
			t = true;
			CDDIndexBuffer* ib = g_IndexTable.getConnectIB( thislevel , TOP , tlv );
			GetRenderDevice().RenderFVF( D3DPT_TRIANGLELIST , thisvb , ib , 0 , TILE_SIDE_VERT*TILE_SIDE_VERT , 0 , ib->getElemsCounts() );
		}
	}
	// 下
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
	// 左
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
	// 右
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
	// 本体
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

	// 获取四个顶点的高度
	float h11 = m_pfHMapInfos[subscript_z * m_un32SideVertCount + subscript_x];
	float h12 = m_pfHMapInfos[subscript_z * m_un32SideVertCount + subscript_x+1];
	float h21 = m_pfHMapInfos[(subscript_z + 1) * m_un32SideVertCount + subscript_x];
	float h22 = m_pfHMapInfos[(subscript_z + 1) * m_un32SideVertCount + subscript_x +1];

	// 插值
	float ha = h11 + x_delta * (h12 - h11);
	float hb = h21 + x_delta * (h22 - h21);

	return (ha + z_delta * (hb - ha));
}

bool CDDITTerrain::IntersectLine( const D3DXVECTOR3& ori , const D3DXVECTOR3& End , D3DXVECTOR3& CollidePoint )
{
	float tmp = 1.0f / m_fScalePlane;
	// 射线发射增长后的临时终点，用于碰撞判断
	D3DXVECTOR3 vTempPos;

	D3DXVECTOR3 vStep = ( End - ori );
	// 射程
	float range = D3DXVec3Length(&vStep) ;
	// 单位增量
	vStep = vStep/range;

	// 一次增加一个格子进行判断
	for( int i=0 ; i < range ; i += m_fScalePlane )
	{
		vTempPos = vStep * i + ori;
		// 取当前射线远端点对应的地形顶点数组下标
		UINT subscript_x = vTempPos.x * tmp;
		UINT subscript_z = vTempPos.z * tmp;

		if( subscript_x >= m_un32SideVertCount-1 || subscript_z >= m_un32SideVertCount-1 )
			return false;

		// 如果射线远点的Y小于对应地形顶点的Y，认为射线已钻入地下
		if( GetHeightByVert( subscript_x , subscript_z ) > vTempPos.y )
		{
			// 在确认钻入地下后，射线回退1个地面方格
			vTempPos -= vStep * m_fScalePlane;

			// 回退后用小的增长量，每次1个单位再次增长射线进行详细判断
			for( int j = 0 ; j < m_fScalePlane * 2 ; j++)
			{
				vTempPos += vStep;

				// 取出精确的碰撞点地面高度（非顶点数组上的）
				float terrain_y = GetHeight( vTempPos.x , vTempPos.z );

				// 判断高度，是否已经钻入地下
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

	// 射线发射增长后的临时终点，用于碰撞判断
	D3DXVECTOR3 vTempPos;

	// dir 本身就是单位矢量，可以用作单位增量
	D3DXVECTOR3 vStep = dir;

	// 一次增加一个格子（默认是32个单位）进行判断
	for( int i=0 ; i < distance ; i += m_fScalePlane )
	{
		vTempPos = vStep * i + ori;
		// 取当前射线远端点对应的地形顶点数组下标
		UINT subscript_x = vTempPos.x * tmp;
		UINT subscript_z = vTempPos.z * tmp;

		if( subscript_x >= m_un32SideVertCount-1 || subscript_z >= m_un32SideVertCount-1 )
			return false;

		// 如果射线远点的Y小于对应地形顶点的Y，认为射线已钻入地下
		if( GetHeightByVert( subscript_x , subscript_z ) > vTempPos.y )
		{
			// 在确认钻入地下后，射线回退1个地面方格
			vTempPos -= vStep * m_fScalePlane;

			// 回退后用小的增长量，每次1个单位再次增长射线进行详细判断
			for( int j = 0 ; j < m_fScalePlane * 2 ; j++)
			{
				vTempPos += vStep;

				// 取出精确的碰撞点地面高度（非顶点数组上的）
				float terrain_y = GetHeight( vTempPos.x , vTempPos.z );

				// 判断高度，是否已经钻入地下
				if( terrain_y >= vTempPos.y )
				{
					CollidePoint = vTempPos;
					// 算出碰撞点到原始点这条线的长度，作为distance返回
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