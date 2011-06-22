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

	// LOD 0 (��ֲ�)������16��ȫ������һ���ģ����������ι��ɵ�һ������
	m_bodyIB[0][0] = new CDDIndexBuffer;
	m_bodyIB[0][0]->create( D3DFMT_INDEX16 , 6 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
	m_bodyIB[0][0]->writeData( 0 , sizeof(BaseTile0) , BaseTile0 );
	
	// LOD 1��
	// body
	for ( int i = 0 ; i < 16 ; i ++ )
	{
		int total_indexes = 0;
		// �ı�û�б仯�ſ��԰�����߼���
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
				// �ı�û�б仯�ſ��԰�����߼���
				if ( !( i & ( 1 << side ) ) )
				{
					m_bodyIB[1][i]->writeData( idx , sizeof(SidesOfLevel1[side]) , SidesOfLevel1[side] );
					idx += 6 * 2;
				}
			}
		}
	}
	// connector��
	for ( int side = 0 ; side < TOTAL_SIDES ; ++side )
	{
		m_connectorIB[1][side][0] = new CDDIndexBuffer;
		m_connectorIB[1][side][0]->create( D3DFMT_INDEX16 , 3 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[1][side][0]->writeData( 0 , sizeof( Connect1to0[side] ) , Connect1to0[side] );
	}
	
	// LOD 2��
	// body
	for ( int body = 0 ; body < 16 ; ++body )
	{
		// Center��ע�������룬��ΪCenter���ܱ߽�LOD�仯��Ӱ��
		// LOD2��Center����4�������Σ�8�������Ρ�24�������㣩
		int total_indexes = 24;
		// �ı�û�б仯�ſ��԰�����߼��루ÿ���߶���2����������2�������Σ�6�������Σ�18�������㣩��
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
		// ����2�����򼶱�0
		m_connectorIB[2][side][0] = new CDDIndexBuffer;
		m_connectorIB[2][side][0]->create( D3DFMT_INDEX16 , 9 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[2][side][0]->writeData( 0 , sizeof(Connect2to0[side]) , Connect2to0[side] );

		// ����2�����򼶱�1
		m_connectorIB[2][side][1] = new CDDIndexBuffer;
		m_connectorIB[2][side][1]->create( D3DFMT_INDEX16 , 12 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[2][side][1]->writeData( 0 , sizeof(Connect2to1[side]) , Connect2to1[side] );
	}

	// LOD 3��
	//body
	for ( int body = 0 ; body < 16 ; ++body )
	{
		// Center��ע�������룬��ΪCenter���ܱ߽�LOD�仯��Ӱ��
		// LOD3��Center����36�������Σ�72�������Ρ�216�������㣩
		int total_indexes = 216;
		// �ı�û�б仯�ſ��԰�����߼��루ÿ���߶���6����������2�������Σ�14�������Σ�42�������㣩��
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
		// ����3�����򼶱�0
		m_connectorIB[3][side][0]->create( D3DFMT_INDEX16 , 21 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][0]->writeData( 0 , sizeof(Connect3to0[side]) , Connect3to0[side] );

		// ����3�����򼶱�1
		m_connectorIB[3][side][1]->create( D3DFMT_INDEX16 , 24 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][1]->writeData( 0 , sizeof(Connect3to1[side]) , Connect3to1[side] );

		// ����3�����򼶱�2
		m_connectorIB[3][side][2]->create( D3DFMT_INDEX16 , 30 , D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT );
		m_connectorIB[3][side][2]->writeData( 0 , sizeof(Connect3to2[side]) , Connect3to2[side] );
	}

	m_isReady = true;

}

CDDIndexBuffer *CDDITIndexTable::getBodyIB(UINT curLevel, bool isTopChanged, bool isLeftChanged, bool isRighttChanged, bool isBottomChanged)
{
	assert( curLevel < 4 );
	// ����Mask
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