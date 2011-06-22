#ifndef __ITQTREETILE9X9_INDEXTABLE_H__
#define __ITQTREETILE9X9_INDEXTABLE_H__

#include "prerequisites.h"
#include "vertexindexbuffer.h"

// 这是tile大小为9 X 9 的实现
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

// IndexBuffer 的数量一定，可以预先制表
class CDDITIndexTable
{
public:
	CDDITIndexTable();
	~CDDITIndexTable();

public:
	// 制表
	void initialize();
	
	// 取得核心的索引
	CDDIndexBuffer *getBodyIB(UINT curLevel,bool isTopChanged,bool isLeftChanged, bool isRighttChanged,bool isBottomChanged);

	// 获取边缘的索引
	CDDIndexBuffer *getConnectIB(UINT curLevel,_ITSIDES side,UINT destLevel);

private:
	/** 被缓存的IB */
	// curlevel , change mask
	// 4--总共的lod级别, 16--16种变化情况
	CDDIndexBuffer*	m_bodyIB[4][16];
	
	// curlevel , side , dstlevel,4 is the level of detail
	CDDIndexBuffer*	m_connectorIB[4][TOTAL_SIDES][4];

	/** 是否已经成表 */
	bool			m_isReady;
};
extern CDDITIndexTable g_IndexTable;




#endif