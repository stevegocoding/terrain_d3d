#include "vertexindexbuffer.h"
#include "RenderDevice.h"

CDDVertexBuffer::CDDVertexBuffer()
{
	m_pVertexBuffer = NULL;
	m_totalBytes = 0;
	
	m_usage = 0;
	m_pool = D3DPOOL_DEFAULT;

	m_vertexFormat = 0;
	m_stride = 0;
}


CDDVertexBuffer::~CDDVertexBuffer()
{
	destroy();
}

void CDDVertexBuffer::destroy()
{
	SafeRelease(m_pVertexBuffer);
	m_vertexFormat = 0;
	m_stride = 0;
	m_totalBytes = 0;
	m_usage = 0;
	m_pool = D3DPOOL_DEFAULT;
}

void CDDVertexBuffer::createFVF(DWORD FVF , UINT numelem , UINT usage , D3DPOOL pool)
{
	destroy();
	m_vertexFormat = FVF;
	m_stride = D3DXGetFVFVertexSize(FVF);
	m_totalBytes = numelem * m_stride;
	m_usage = usage;
	m_pool = pool;

	HRESULT hr;
	if ( FAILED( hr=GetRenderDevice().GetD3DDevice()->CreateVertexBuffer(m_totalBytes,usage,0,pool,&m_pVertexBuffer,NULL) ) )
		const char *error = DXGetErrorDescription(hr);
}

void CDDVertexBuffer::createStride(USHORT stride, UINT numelem, UINT usage,D3DPOOL pool)
{
	destroy();
	m_vertexFormat = 0;
	m_stride = stride;
	m_totalBytes = numelem * m_stride;
	m_usage = usage;
	m_pool = pool;

	HRESULT hr;
	if ( FAILED( hr=GetRenderDevice().GetD3DDevice()->CreateVertexBuffer(m_totalBytes,usage,0,pool,&m_pVertexBuffer,NULL) ) )
		const char *error = DXGetErrorDescription(hr);
}

void *CDDVertexBuffer::lock(UINT &offset,UINT &length,UINT options)
{
	if (offset > m_totalBytes)
		offset = 0;
	if (offset + length > m_totalBytes)
		length = m_totalBytes - offset;

	UINT lockOptions;
	// Discard不能锁住Dynamic内存
	if ( !( m_usage & D3DUSAGE_DYNAMIC ) && ( options == D3DLOCK_DISCARD || options == D3DLOCK_NOOVERWRITE ) )
		lockOptions = 0;

	void *pBuffer = 0;
	if ( FAILED(m_pVertexBuffer->Lock(offset,length,&pBuffer,options)) )
		return NULL;

	return pBuffer;
}

void CDDVertexBuffer::unlock()
{
	m_pVertexBuffer->Unlock();
}

void CDDVertexBuffer::readData(UINT offset,UINT length,void *pDest)
{
	void* pSrc = lock( offset , length , D3DLOCK_READONLY );

	memcpy( pDest , pSrc , length );

	unlock();
}

void CDDVertexBuffer::writeData(UINT offset , UINT length , const void* pSource , BOOL discardWholeBuffer)
{
	// 锁存，注意Lock里会更改length到一个比较合适的值
	void* pDst = lock( offset , length , discardWholeBuffer? D3DLOCK_DISCARD : 0  );

	memcpy( pDst , pSource , length );

	unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////


CDDIndexBuffer::CDDIndexBuffer()
{
	m_pIndexBuffer = NULL;
}

CDDIndexBuffer::~CDDIndexBuffer()
{
	destroy();
}

void CDDIndexBuffer::destroy()
{
	SafeRelease(m_pIndexBuffer);
}

void CDDIndexBuffer::create(D3DFORMAT format, UINT numelem, UINT usage, D3DPOOL pool)
{
	destroy();
	m_stride = ( (format == D3DFMT_INDEX16)? sizeof(WORD) : sizeof(DWORD) );
	m_totalBytes = m_stride * numelem;
	m_usage = usage;
	m_pool = pool;
	
	HRESULT hr;
	if( FAILED( hr = GetRenderDevice().GetD3DDevice()->CreateIndexBuffer( m_totalBytes , usage , format , pool , &m_pIndexBuffer , NULL ) ) )
	{
		const char* error = DXGetErrorDescription( hr );
		return;
	}
}

void *CDDIndexBuffer::lock(UINT &offset,UINT &length,UINT options)
{
	// 如果offset不合理，从头取
	if ( offset > m_totalBytes )
	{ offset = 0; }
	// 如果大于Count，修改length
	if( offset + length > m_totalBytes )
	{ length = m_totalBytes - offset; }

	UINT lockOpts;
	// Discard不能锁住Dynamic内存
	if ( !( m_usage & D3DUSAGE_DYNAMIC ) && ( options == D3DLOCK_DISCARD ) )
	{ lockOpts = 0; }

	void* pBuf = NULL;

	if( FAILED ( m_pIndexBuffer->Lock( offset , length , &pBuf , options ) ) )
	{ return NULL; }

	return pBuf;
}

void CDDIndexBuffer::unlock()
{
	m_pIndexBuffer->Unlock();
}

void CDDIndexBuffer::readData(UINT offset,UINT length,void *pDest)
{
	// 锁存，注意Lock里会更改length到一个比较合适的值
	void* pSrc = lock( offset , length , D3DLOCK_READONLY );

	memcpy( pDest , pSrc , length );

	unlock();
}

void CDDIndexBuffer::writeData(UINT offset,UINT length,const void *pSource,BOOL discardWholeBuffer)
{
	// 锁存，注意Lock里会更改length到一个比较合适的值
	void* pDst = lock( offset , length , discardWholeBuffer? D3DLOCK_DISCARD : 0  );

	memcpy( pDst , pSource , length );

	unlock();
}
