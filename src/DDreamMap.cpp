#include "DDreamMap.h"
#pragma warning (disable : 4311)



void *CDDMap::lookUp(void *key) const
{
	int beginPos = 0,endPos = m_num-1,middle = 0;
	unsigned t = (unsigned)key;

	// 二分查找
	while(beginPos <= endPos) {
		middle=(beginPos + endPos)/2;
		unsigned temp=(unsigned)m_ptr[middle*2];
		if (temp==t) return m_ptr[middle*2+1];
		else if (temp>t) endPos = middle-1;
		else beginPos = middle+1;
	}
	return 0;
}

void CDDMap::insert(void *key,void *val)
{
	int beginPos=0,endPos=m_num-1,middle;
	unsigned t=(unsigned)key;
	while(beginPos<=endPos) {
		middle=(beginPos+endPos)/2;
		unsigned temp=(unsigned)m_ptr[middle*2];
		if (temp==t) {
			m_ptr[middle*2+1]=val;
			return;
		}
		else if (temp>t) endPos=middle-1;
		else beginPos=middle+1;
	}
// 在 beginPos 处插入
	if (m_num>=m_size) {
		m_size+=BLOCK_SIZE;
		++m_num;
		void **newptr=new void*[m_size*2];
		int i;
		for (i=0;i<beginPos*2;i++)
			newptr[i]=m_ptr[i];
		newptr[beginPos*2]=key;
		newptr[beginPos*2+1]=val;
		for (i=beginPos*2;i<m_num*2;i++)
			newptr[i+2]=m_ptr[i];
		m_ptr=newptr;
	}
	else {
		++m_num;
		for (int i=m_num*2-1;i>beginPos*2+1;i--)
			m_ptr[i]=m_ptr[i-2];
		m_ptr[beginPos*2]=key;
		m_ptr[beginPos*2+1]=val;
	}
	return;
}

void CDDMap::del(void *key)
{
	int begin=0,end=m_num-1,middle;
	unsigned t=(unsigned)key;
	while(begin<=end) {
		middle=(begin+end)/2;
		unsigned temp=(unsigned)m_ptr[middle*2];
		if (temp==t) {
			--m_num;
			for (int i=middle*2;i<m_num*2;i++)
				m_ptr[i]=m_ptr[i+2];
			return;
		}
		else if (temp>t) end=middle-1;
		else begin=middle+1;
	}
}