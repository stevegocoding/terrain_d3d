#ifndef __VERTEX_INDEX_BUFFER_H__
#define __VERTEX_INDEX_BUFFER_H__

#include "prerequisites.h"


class CDDVertexBuffer
{
	friend class CRenderDevice;
public:
	CDDVertexBuffer();
	~CDDVertexBuffer();

public:
	inline unsigned int getTotalBytes() { return m_totalBytes; }
	inline unsigned int getElemsCounts() { return m_totalBytes / m_stride; }

	// 根据FVF定点格式创建vertexbuffer
	void createFVF( DWORD FVF , UINT numelem , UINT usage , D3DPOOL pool);
	// 根据每个顶点格式长度(步长)创建vertexbuffer
	void createStride( USHORT stride, UINT numelem, UINT usage, D3DPOOL pool);
	// 销毁vertexbuffer
	void destroy(); 

	// lock
	void *lock(UINT& offset , UINT& length , UINT options);
	// unlock 
	void unlock();

	// read and write
	void readData(UINT offset , UINT numBYTE , void* pDest);
	void writeData(UINT offset , UINT numBYTE , const void* pSource , BOOL discardWholeBuffer = false);


private:
	// VB 接口
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	// 总长度(by bytes)
	unsigned int m_totalBytes;
	
	/** Usage和Pool，用于Lock */
	UINT m_usage;
	D3DPOOL	m_pool;
	
	/** 顶点的格式和含义
	 *	由各种 D3DFVF 常量组织而成
	 */
	DWORD m_vertexFormat;

	/** Stride，用于SetStreamSource */
	USHORT m_stride;




};

class CDDIndexBuffer
{
	friend class CRenderDevice;
public:
	CDDIndexBuffer();
	~CDDIndexBuffer();

public:
	inline unsigned int getTotalBytes() { return m_totalBytes; }
	inline unsigned int getElemsCounts() { return m_totalBytes / m_stride; }
	
	/**  */
	void create( D3DFORMAT format , UINT numelem , UINT usage , D3DPOOL pool );
	void destroy();
	/**  */
	void *lock( UINT& offset , UINT& numBYTE , UINT options );
	void unlock();
	/**  */
	void readData( UINT offset , UINT numBYTE , void* pDest );
	void writeData( UINT offset , UINT numBYTE , const void* pSource , BOOL discardWholeBuffer = false );

private:
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	UINT m_totalBytes;

	UINT m_stride;

	UINT m_usage;
	D3DPOOL m_pool;
};





#endif