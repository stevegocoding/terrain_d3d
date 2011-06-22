#ifndef __DDTEXTURE_H__
#define __DDTEXTURE_H__

#include "prerequisites.h"

class CDDTexture
{
public:
	CDDTexture();
	~CDDTexture();
public:
	bool createFromFile(const char *fileName);
	bool createFromMemory(BYTE *pData,
						unsigned int width,
						unsigned int height,
						D3DFORMAT format,
						D3DTEXTUREFILTERTYPE mipFilter = D3DTEXF_LINEAR);
	void destroy();
	// bool createFromMemoryEx();
	inline LPDIRECT3DTEXTURE9 getD3DTexture() {
		return m_pTexture;
	}

private:
	// the pointer of the IDirect3DTexture9 interface
	LPDIRECT3DTEXTURE9 m_pTexture;

};



#endif 