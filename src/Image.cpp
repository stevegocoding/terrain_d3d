#include "image.h"


void CDDImage::unload()
{
	m_height = m_width = 0;
	m_pBuffer = 0;
	m_bytesPerPixel = 0;
	SafeDeleteArray(m_pBuffer);
}

bool CDDImage::loadBmp(const char *fileName)
{
	typedef struct 
	{
		unsigned short bfType;           
		unsigned int   bfSize;           
		unsigned short bfReserved1;      
		unsigned short bfReserved2;      
		unsigned int   bfOffBits;        
	} t_bmpFHEAD;

	typedef struct  
	{
		unsigned int   biSize;           
		int            biWidth;          
		int            biHeight;         
		unsigned short biPlanes;         
		unsigned short biBitCount;       
		unsigned int   biCompression;    
		unsigned int   biSizeImage;      
		int            biXPelsPerMeter;  
		int            biYPelsPerMeter;  
		unsigned int   biClrUsed;        
		unsigned int   biClrImportant;   

	} t_bmpIHEAD;
	
	int x, y;
	t_bmpFHEAD h1;
	t_bmpIHEAD h2;
	// ���ļ�����FILE*  fopen(char *fname , char *mode)	mode:r , w , a , rb, wb , ab , r+ , w+ , a+ , rb+ , wb+ , ab+�ֱ��Ǵ��ı��������ƣ�ֻ����ֻд����Ӷ�д
	FILE *fp = fopen(fileName, "rb");
	if(!fp) {
		return false;
	}

	// �����ļ���ʶ����int fread(void* buf���ַ������ , int size���ֶγ��� , int count�۶����ֶ����� , FILE* stream��ָ��Դ��������)
	fread(&h1.bfType, sizeof(h1.bfType),1,fp);
	
	// �ж��ļ������Ƿ�BMP
	if(h1.bfType != 0x4D42) {
		fclose(fp);
		return false;
	}

	// �ļ�ͷ������Ϣ
	fread(&h1.bfSize, sizeof(h1.bfSize),1, fp);
	fread(&h1.bfReserved1,sizeof(h1.bfReserved1),1, fp);
	fread(&h1.bfReserved2,sizeof(h1.bfReserved2),1, fp);
	fread(&h1.bfOffBits,sizeof(h1.bfOffBits),1, fp);
	// �ļ�ͷ��Ϣ��ȡ���
	
	// ������Ϣͷ
	fread(&h2,	sizeof(h2),	1, fp);
	// ��Ϣͷ�������

	// ��ȡ��Ⱥ͸߶ȵ����ݳ�Ա��
	m_width  = h2.biWidth;
	m_height = h2.biHeight;

	// �������8λ��24λ���˳�
	if(h2.biBitCount !=8 && h2.biBitCount !=24)
	{
		fclose(fp);
		return false;
	}

	// �ж��Ƿ����ɫ�壬ע�⣺�ļ�ͷ����Ϣͷ�ֱ�Ϊ14��40�ֽ�
	bool bHavePalette;
	if(h1.bfOffBits == 54)
		bHavePalette = false;
	else
		bHavePalette = true;

	// ���ָ�ͼΪ8λ�Ҷ�ͼ:bitcountΪ8���޵�ɫ��
	if(h2.biBitCount == 8 && !bHavePalette)
	{
		m_bytesPerPixel = 1;
	}
	// ���ָ�ͼΪ8λ��256��ɫ��ͼ:bitcountΪ8���е�ɫ��
	if(h2.biBitCount == 8 && bHavePalette)
	{
		// ����ɫ��ͼ��ת��RGB
		m_bytesPerPixel = 3;
	}
	// ���ָ�ͼΪ24λRGBͼ:bitcountΪ24
	if(h2.biBitCount == 24 )
	{
		// ���RGB��ʽ
		m_bytesPerPixel = 3;
	}

	// Ϊ���ݳ�Աm_pBuffer����ռ�
	m_pBuffer = new BYTE[(m_width*m_height)*m_bytesPerPixel];
	if(!m_pBuffer) 
		return false;

	// �����ȡ��ɫ��Ϣ��Ҫ����ʱ����
	BYTE *color = NULL;
	color = new BYTE[m_bytesPerPixel];
	if(!color)
		return false;
	memset(color, 0, sizeof(BYTE) * m_bytesPerPixel);
	BYTE swpclr;
	
	// ���¶�ȡ������Ϣ�����ݳ�Աm_pBuffer��
	// ���ָ�ͼΪ8λ�Ҷ�ͼ:bitcountΪ8���޵�ɫ��
	if(h2.biBitCount == 8 && !bHavePalette)
	{
		// ������ɫ��Ϣ��תΪRGB��洢
		for(y=0; y<m_height; y++) 
		{
			for(x=0; x<m_width; x++) 
			{
				fread (color, sizeof(BYTE), 1, fp);
				// �������������ɫ��Ϣ���뵽���ݳ�Աm_pBuffer��
				setColor (x, y, color);
			}

			//Add padding BYTEs?????????
			//FIX: Move filepointer instead
			//for (int a=0; a<(4-((3*m_un16Width)%4))%4; a++)
			//	fread (&swpclr, sizeof(BYTE), 1, fp);
		}

	}
	
	// ���ָ�ͼΪ8λ��256ɫ��ɫ��ͼ:bitcountΪ8���е�ɫ��
	if(h2.biBitCount == 8 && bHavePalette)
	{
		// ����ɫ��ͼ��ת��24λRGB��ʽ
		// �ȶ����ɫ��,��ɫ��Ϊ4���ֽ�R��G��B����־λ
		// �ļ�ͷ����Ϣͷ��54�ֽڣ�������ƫ������ȥ54Ϊ��ɫ���ֽ���
		BYTE *ColorPlanes;
		ColorPlanes = new BYTE[(h1.bfOffBits - 54)];
		fread (ColorPlanes, sizeof(BYTE) * (h1.bfOffBits - 54) , 1, fp);

		// ����ɫ���е�BGRתΪRGB
		for(x=0; (DWORD)x < (h1.bfOffBits -54) ; x+=4) 
		{
			swpclr=ColorPlanes[x];
			ColorPlanes[x]=ColorPlanes[x+2];
			ColorPlanes[x+2]=swpclr;
		}
		// �����ɫ��������Ϣ��תΪRGB��洢
		for(y=0; y<m_height; y++) 
		{
			for(x=0; x<m_width; x++) 
			{
				BYTE index[1] = {0};
				fread (index, sizeof(BYTE), 1, fp);
				color[0] = ColorPlanes [index[0] * 4];
				color[1] = ColorPlanes [index[0] * 4+1];
				color[2] = ColorPlanes [index[0] * 4+2];

				// �������������ɫ��Ϣ���뵽���ݳ�Աm_pBuffer��
				setColor (x, y, color);
			}

			//Add padding BYTEs?????????
			//FIX: Move filepointer instead
			//for (int a=0; a<(4-((3*m_un16Width)%4))%4; a++)
			//	fread (&swpclr, sizeof(BYTE), 1, fp);
		}
		delete []ColorPlanes;
		ColorPlanes = NULL;
	}
	
	// ���ָ�ͼΪ24λRGBͼ
	if( h2.biBitCount == 24 ) {
		// ����ɫ��ͼ��ת��24λRGB��ʽ
		for(y=0; y<m_height; y++) {
			for(x=0; x<m_width; x++) {
				fread (color, sizeof(BYTE)*3, 1, fp);
				//���� BGR to RGB
				swpclr=color[0];
				color[0]=color[2];
				color[2]=swpclr;

				// �������������ɫ��Ϣ���뵽���ݳ�Աm_pBuffer��
				setColor (x, y, color);
			}
		}
	}

	// �ر��ļ�
	fclose(fp);
	// �ͷ���ʱ������ɫ�ռ�
	if(color)
	{	
		delete [] color;
		color = NULL;
	}

	flipMap( m_pBuffer , m_bytesPerPixel * m_width , m_height );

	return true;
}

void CDDImage::flipMap(BYTE *pImage,int pitch,int height)
{
	BYTE *buffer; // used to perform the image processing
	int index;     // looping index

	// allocate the temporary buffer
	if ( !( buffer = new BYTE[ pitch * height] ) )
		return;

	// copy image to work area
	memcpy( buffer , pImage , pitch * height );

	// flip vertically
	for (index=0; index < height; index++)
	{
		memcpy( &pImage[((height - 1) - index)*pitch] , &buffer[index*pitch], pitch );
	}

	// release the memory
	delete buffer;

	// return success
	return;
}

void CDDImage::setColor(int x,int y,BYTE *color)
{
	if((x < m_width) && (y < m_height)) {
		for (int p=0; p<m_bytesPerPixel; p++) {
			m_pBuffer[(x + (y*m_width))*m_bytesPerPixel + p] = color[p];
		}
	}
}

DWORD CDDImage::getColor32(int x,int y)
{
	DWORD returnColor;
	BYTE* pColor = (BYTE*)&returnColor;
	if((x < m_width) && (y < m_height) && (x >= 0) && (y >= 0)) 
	{
		for ( int p=0; p<m_bytesPerPixel; p++ )
		{
			pColor[p] = m_pBuffer[(x + (y*m_width))*m_bytesPerPixel + p];
		}
	}
	return returnColor;
}

BYTE CDDImage::getColor8(int x, int y)
{
	BYTE returnColor;
	if((x < m_width) && (y < m_height) && (x >= 0) && (y >= 0)) 
	{
		returnColor = m_pBuffer[(x + (y*m_width))*m_bytesPerPixel];
	}
	return returnColor;
}

// ����D3DFORMAT��Ӧ�ĸ�ʽ����(�ֽ���)
UINT GetD3DFormatLength( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_UNKNOWN:
		return 0;
	case D3DFMT_L8:
	case D3DFMT_A8:
	case D3DFMT_P8:
	case D3DFMT_A4L4:
		return 1;
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
		return 2;
	case D3DFMT_R8G8B8:
		return 3;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
		return 4;
	default:
		return 0;
	}
}
