#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "prerequisites.h"

class CDDImage
{
public:
	CDDImage() {
		m_height = m_width = 0;
		m_pBuffer = 0;
		m_bytesPerPixel = 0;
	}
	~CDDImage() {
		unload();
	}

public:
	bool loadBmp(const char *fileName);
	void unload();
	void flipMap(BYTE *pImage,int pitch /*bytesPerLine*/,int height); 
	void setColor(int x,int y,BYTE *color);
	BYTE getColor8(int x,int y);
	DWORD getColor32(int x,int y);

	inline UINT getHeight() {
		return m_height;
	}
	inline UINT getWidth() {
		return m_width;
	}
	
private:
	BYTE *m_pBuffer;
	
	D3DFORMAT m_imageFormat;

	UINT m_bytesPerPixel;

	UINT m_height, m_width;
};

extern UINT GetD3DFormatLength( D3DFORMAT fmt );


#endif