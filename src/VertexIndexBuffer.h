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

	// ����FVF�����ʽ����vertexbuffer
	void createFVF( DWORD FVF , UINT numelem , UINT usage , D3DPOOL pool);
	// ����ÿ�������ʽ����(����)����vertexbuffer
	void createStride( USHORT stride, UINT numelem, UINT usage, D3DPOOL pool);
	// ����vertexbuffer
	void destroy(); 

	// lock
	void *lock(UINT& offset , UINT& length , UINT options);
	// unlock 
	void unlock();

	// read and write
	void readData(UINT offset , UINT numBYTE , void* pDest);
	void writeData(UINT offset , UINT numBYTE , const void* pSource , BOOL discardWholeBuffer = false);


private:
	// VB �ӿ�
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	// �ܳ���(by bytes)
	unsigned int m_totalBytes;
	
	/** Usage��Pool������Lock */
	UINT m_usage;
	D3DPOOL	m_pool;
	
	/** ����ĸ�ʽ�ͺ���
	 *	�ɸ��� D3DFVF ������֯����
	 */
	DWORD m_vertexFormat;

	/** Stride������SetStreamSource */
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