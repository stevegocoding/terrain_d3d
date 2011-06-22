#include "ITQTreeTile9X9IndexTable.h"
#include "ITQTreeTile9X9IndexData.h"
#include "vertexindexbuffer.h"



CDDITIndexTable g_IndexTable;

CDDITIndexTable::CDDITIndexTable()
{
	for ( int i = 0 ; i < 4 ; i ++ ) { 
		for ( int j = 0 ; j < 16 ; j++ ) { 
			m_bodyIB[i][j] = NULL; 
		} 
	}

	for ( int i = 0 ; i < 4 ; i ++ ) { 
		for ( int j = 0 ; j < 4 ; j++ ) { 
			for ( int k = 0 ; k < 4 ; k++ ) { 
				m_connectorIB[i][j][k] = NULL; 
			} 
		} 
	}

	m_isReady = false;
}

CDDITIndexTable::~CDDITIndexTable()
{
	for ( int i = 0 ; i < 4 ; i ++ ) { 
		for ( int j = 0 ; j < 16 ; j++ ) { 
			SafeDelete(m_bodyIB[i][j]); 
		} 
	}

	for ( int i = 0 ; i < 4 ; i ++ ) { 
		for ( int j = 0 ; j < 4 ; j++ ) { 
			for ( int k = 0 ; k < 4 ; k++ ) { 
				SafeDelete(m_connectorIB[i][j][k]); 
			} 
		} 
	}

	m_isReady = false;
}

void CDDITIndexTable::initialize()
{
	if (m_isReady)
		return;

	// LOD 0 (最粗糙)：所有16个全部都是一样的：两个三角形构成的一个方形
	m_bodyIB[0][0] = new CDDIndexBuffer;
	m_bodyIB[0][0]->create( D3DFMT_INDEX16 , 6 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
	m_bodyIB[0][0]->writeData( 0 , sizeof(BaseTile0) , BaseTile0 );
	
	// LOD 1：
	// body
	for ( int i = 0 ; i < 16 ; i ++ )
	{
		int total_indexes = 0;
		// 哪边没有变化才可以把这个边计入
		if ( !( i & ( 1 << TOP ) ) )	total_indexes += 6;
		if ( !( i & ( 1 << LEFT ) ) )	total_indexes += 6;
		if ( !( i & ( 1 << RIGHT ) ) )	total_indexes += 6;
		if ( !( i & ( 1 << BOTTOM ) ) )	total_indexes += 6;

		if ( total_indexes )
		{
			m_bodyIB[1][i] = new CDDIndexBuffer;
			m_bodyIB[1][i]->create( D3DFMT_INDEX16 , total_indexes , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );

			int idx = 0;
			for ( int side=0 ; side < TOTAL_SIDES ; ++side )
			{
				// 哪边没有变化才可以把这个边计入
				if ( !( i & ( 1 << side ) ) )
				{
					m_bodyIB[1][i]->writeData( idx , sizeof(SidesOfLevel1[side]) , SidesOfLevel1[side] );
					idx += 6 * 2;
				}
			}
		}
	}
	// connector：
	for ( int side = 0 ; side < TOTAL_SIDES ; ++side )
	{
		m_connectorIB[1][side][0] = new CDDIndexBuffer;
		m_connectorIB[1][side][0]->create( D3DFMT_INDEX16 , 3 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[1][side][0]->writeData( 0 , sizeof( Connect1to0[side] ) , Connect1to0[side] );
	}
	
	// LOD 2：
	// body
	for ( int body = 0 ; body < 16 ; ++body )
	{
		// Center将注定被记入，因为Center不受边角LOD变化的影响
		// LOD2的Center包括4个正方形（8个三角形、24个索引点）
		int total_indexes = 24;
		// 哪边没有变化才可以把这个边计入（每个边都有2个正方形又2个三角形（6个三角形，18个索引点））
		if ( !( body & ( 1 << TOP ) ) )	total_indexes += 18;
		if ( !( body & ( 1 << LEFT ) ) )	total_indexes += 18;
		if ( !( body & ( 1 << RIGHT ) ) )	total_indexes += 18;
		if ( !( body & ( 1 << BOTTOM ) ) )	total_indexes += 18;

		m_bodyIB[2][body] = new CDDIndexBuffer;
		m_bodyIB[2][body]->create( D3DFMT_INDEX16 , total_indexes , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_bodyIB[2][body]->writeData( 0 , sizeof(Level2_Center) , Level2_Center );
		int idx = 24 * 2;
		for ( int side = 0 ; side < TOTAL_SIDES ; side++ )
		{
			if ( !(body & ( 1 << side ) ) )
			{
				m_bodyIB[2][body]->writeData( idx , sizeof(SidesOfLevel2[side]) , SidesOfLevel2[side] );
				idx += 18 * 2;
			}
		}
	}
	// connector
	for ( int side = 0 ; side < TOTAL_SIDES ; ++side )
	{
		// 级别2连接向级别0
		m_connectorIB[2][side][0] = new CDDIndexBuffer;
		m_connectorIB[2][side][0]->create( D3DFMT_INDEX16 , 9 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[2][side][0]->writeData( 0 , sizeof(Connect2to0[side]) , Connect2to0[side] );

		// 级别2连接向级别1
		m_connectorIB[2][side][1] = new CDDIndexBuffer;
		m_connectorIB[2][side][1]->create( D3DFMT_INDEX16 , 12 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[2][side][1]->writeData( 0 , sizeof(Connect2to1[side]) , Connect2to1[side] );
	}

	// LOD 3：
	//body
	for ( int body = 0 ; body < 16 ; ++body )
	{
		// Center将注定被记入，因为Center不受边角LOD变化的影响
		// LOD3的Center包括36个正方形（72个三角形、216个索引点）
		int total_indexes = 216;
		// 哪边没有变化才可以把这个边计入（每个边都有6个正方形又2个三角形（14个三角形，42个索引点））
		if ( !( body & ( 1 << TOP ) ) )		total_indexes += 42;
		if ( !( body & ( 1 << LEFT ) ) )	total_indexes += 42;
		if ( !( body & ( 1 << RIGHT ) ) )	total_indexes += 42;
		if ( !( body & ( 1 << BOTTOM ) ) )	total_indexes += 42;

		m_bodyIB[3][body] = new CDDIndexBuffer;
		m_bodyIB[3][body]->create( D3DFMT_INDEX16 , total_indexes , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_bodyIB[3][body]->writeData( 0 , sizeof(Level3_Center) , Level3_Center );
		int idx = 216 * 2;
		for ( int side = 0 ; side < TOTAL_SIDES ; side++ )
		{
			if ( !(body & ( 1 << side ) ) )
			{
				m_bodyIB[3][body]->writeData( idx , sizeof(SidesOfLevel3[side]) , SidesOfLevel3[side] );
				idx += 42 * 2;
			}
		}
	}
	// connector
	for ( int side = 0 ; side < TOTAL_SIDES ; ++side )
	{
		m_connectorIB[3][side][0] = new CDDIndexBuffer;
		m_connectorIB[3][side][1] = new CDDIndexBuffer;
		m_connectorIB[3][side][2] = new CDDIndexBuffer;
		// 级别3连接向级别0
		m_connectorIB[3][side][0]->create( D3DFMT_INDEX16 , 21 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][0]->writeData( 0 , sizeof(Connect3to0[side]) , Connect3to0[side] );

		// 级别3连接向级别1
		m_connectorIB[3][side][1]->create( D3DFMT_INDEX16 , 24 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][1]->writeData( 0 , sizeof(Connect3to1[side]) , Connect3to1[side] );

		// 级别3连接向级别2
		m_connectorIB[3][side][2]->create( D3DFMT_INDEX16 , 30 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][2]->writeData( 0 , sizeof(Connect3to2[side]) , Connect3to2[side] );
	}

	m_isReady = true;

}

CDDIndexBuffer *CDDITIndexTable::getBodyIB(UINT curLevel, bool isTopChanged, bool isLeftChanged, bool isRighttChanged, bool isBottomChanged)
{
	assert( curLevel < 4 );
	// 生成Mask
	int mask = 0;
	if ( isTopChanged )	{ mask |= 1 << TOP; }
	if ( isLeftChanged )	{ mask |= 1 << LEFT; }
	if ( isRighttChanged )	{ mask |= 1 << RIGHT; }
	if ( isBottomChanged )	{ mask |= 1 << BOTTOM; }

	return m_bodyIB[curLevel][mask];
}

CDDIndexBuffer *CDDITIndexTable::getConnectIB(UINT curLevel, _ITSIDES side, UINT destLevel)
{
	assert( curLevel < 4 );
	assert( destLevel < 3 );
	return m_connectorIB[curLevel][side][destLevel];
}