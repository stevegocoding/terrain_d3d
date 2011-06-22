#ifndef __ITQTREETILE9X9_INDEXTABLE_H__
#define __ITQTREETILE9X9_INDEXTABLE_H__

#include "prerequisites.h"
#include "vertexindexbuffer.h"

// ����tile��СΪ9 X 9 ��ʵ��
#define	TILE_SIDE_VERT	9
#define	TILE_TOTAL_VERT	81

enum _ITSIDES
{
	TOP=0,
	LEFT,
	RIGHT,
	BOTTOM,
	TOTAL_SIDES
};

// IndexBuffer ������һ��������Ԥ���Ʊ�
class CDDITIndexTable
{
public:
	CDDITIndexTable();
	~CDDITIndexTable();

public:
	// �Ʊ�
	void initialize();
	
	// ȡ�ú��ĵ�����
	CDDIndexBuffer *getBodyIB(UINT curLevel,bool isTopChanged,bool isLeftChanged, bool isRighttChanged,bool isBottomChanged);

	// ��ȡ��Ե������
	CDDIndexBuffer *getConnectIB(UINT curLevel,_ITSIDES side,UINT destLevel);

private:
	/** �������IB */
	// curlevel , change mask
	// 4--�ܹ���lod����, 16--16�ֱ仯���
	CDDIndexBuffer*	m_bodyIB[4][16];
	
	// curlevel , side , dstlevel,4 is the level of detail
	CDDIndexBuffer*	m_connectorIB[4][TOTAL_SIDES][4];

	/** �Ƿ��Ѿ��ɱ� */
	bool			m_isReady;
};
extern CDDITIndexTable g_IndexTable;




#endif