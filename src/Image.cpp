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
	// 打开文件／／FILE*  fopen(char *fname , char *mode)	mode:r , w , a , rb, wb , ab , r+ , w+ , a+ , rb+ , wb+ , ab+分别是打开文本，二进制，只读，只写，添加读写
	FILE *fp = fopen(fileName, "rb");
	if(!fp) {
		return false;
	}

	// 读入文件标识／／int fread(void* buf［字符数组］ , int size［字段长］ , int count［读入字段数］ , FILE* stream［指向源数据流］)
	fread(&h1.bfType, sizeof(h1.bfType),1,fp);
	
	// 判断文件类型是否BMP
	if(h1.bfType != 0x4D42) {
		fclose(fp);
		return false;
	}

	// 文件头其他信息
	fread(&h1.bfSize, sizeof(h1.bfSize),1, fp);
	fread(&h1.bfReserved1,sizeof(h1.bfReserved1),1, fp);
	fread(&h1.bfReserved2,sizeof(h1.bfReserved2),1, fp);
	fread(&h1.bfOffBits,sizeof(h1.bfOffBits),1, fp);
	// 文件头信息读取完毕
	
	// 读入信息头
	fread(&h2,	sizeof(h2),	1, fp);
	// 信息头读入完毕

	// 获取宽度和高度到数据成员中
	m_width  = h2.biWidth;
	m_height = h2.biHeight;

	// 如果不是8位或24位则退出
	if(h2.biBitCount !=8 && h2.biBitCount !=24)
	{
		fclose(fp);
		return false;
	}

	// 判断是否带调色板，注意：文件头和信息头分别为14、40字节
	bool bHavePalette;
	if(h1.bfOffBits == 54)
		bHavePalette = false;
	else
		bHavePalette = true;

	// 发现该图为8位灰度图:bitcount为8且无调色板
	if(h2.biBitCount == 8 && !bHavePalette)
	{
		m_bytesPerPixel = 1;
	}
	// 发现该图为8位带256调色板图:bitcount为8且有调色板
	if(h2.biBitCount == 8 && bHavePalette)
	{
		// 带颜色的图都转成RGB
		m_bytesPerPixel = 3;
	}
	// 发现该图为24位RGB图:bitcount为24
	if(h2.biBitCount == 24 )
	{
		// 存成RGB格式
		m_bytesPerPixel = 3;
	}

	// 为数据成员m_pBuffer分配空间
	m_pBuffer = new BYTE[(m_width*m_height)*m_bytesPerPixel];
	if(!m_pBuffer) 
		return false;

	// 定义读取颜色信息需要的临时变量
	BYTE *color = NULL;
	color = new BYTE[m_bytesPerPixel];
	if(!color)
		return false;
	memset(color, 0, sizeof(BYTE) * m_bytesPerPixel);
	BYTE swpclr;
	
	// 以下读取像素信息到数据成员m_pBuffer中
	// 发现该图为8位灰度图:bitcount为8且无调色板
	if(h2.biBitCount == 8 && !bHavePalette)
	{
		// 读入颜色信息，转为RGB后存储
		for(y=0; y<m_height; y++) 
		{
			for(x=0; x<m_width; x++) 
			{
				fread (color, sizeof(BYTE), 1, fp);
				// 将读入的像素颜色信息加入到数据成员m_pBuffer中
				setColor (x, y, color);
			}

			//Add padding BYTEs?????????
			//FIX: Move filepointer instead
			//for (int a=0; a<(4-((3*m_un16Width)%4))%4; a++)
			//	fread (&swpclr, sizeof(BYTE), 1, fp);
		}

	}
	
	// 发现该图为8位带256色调色板图:bitcount为8且有调色板
	if(h2.biBitCount == 8 && bHavePalette)
	{
		// 带颜色的图都转成24位RGB格式
		// 先读入调色板,调色板为4个字节R、G、B、标志位
		// 文件头和信息头共54字节，数据区偏移量减去54为调色板字节数
		BYTE *ColorPlanes;
		ColorPlanes = new BYTE[(h1.bfOffBits - 54)];
		fread (ColorPlanes, sizeof(BYTE) * (h1.bfOffBits - 54) , 1, fp);

		// 将调色板中的BGR转为RGB
		for(x=0; (DWORD)x < (h1.bfOffBits -54) ; x+=4) 
		{
			swpclr=ColorPlanes[x];
			ColorPlanes[x]=ColorPlanes[x+2];
			ColorPlanes[x+2]=swpclr;
		}
		// 读入调色板索引信息，转为RGB后存储
		for(y=0; y<m_height; y++) 
		{
			for(x=0; x<m_width; x++) 
			{
				BYTE index[1] = {0};
				fread (index, sizeof(BYTE), 1, fp);
				color[0] = ColorPlanes [index[0] * 4];
				color[1] = ColorPlanes [index[0] * 4+1];
				color[2] = ColorPlanes [index[0] * 4+2];

				// 将读入的像素颜色信息加入到数据成员m_pBuffer中
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
	
	// 发现该图为24位RGB图
	if( h2.biBitCount == 24 ) {
		// 带颜色的图都转成24位RGB格式
		for(y=0; y<m_height; y++) {
			for(x=0; x<m_width; x++) {
				fread (color, sizeof(BYTE)*3, 1, fp);
				//交换 BGR to RGB
				swpclr=color[0];
				color[0]=color[2];
				color[2]=swpclr;

				// 将读入的像素颜色信息加入到数据成员m_pBuffer中
				setColor (x, y, color);
			}
		}
	}

	// 关闭文件
	fclose(fp);
	// 释放临时像素颜色空间
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

// 计算D3DFORMAT对应的格式长度(字节数)
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
