#ifndef _DDMAP_H_
#	define _DDMAP_H_

#if _MSC_VER > 1000
#	pragma once
#endif

// 一个简单的map
class CDDMap
{
public:
	CDDMap() : m_size(BLOCK_SIZE),m_num(0),m_ptr(new void*[BLOCK_SIZE*2]) {}
	~CDDMap() { delete[] m_ptr; }
public:
	// 查找
	void *lookUp(void *key) const;
	void insert(void *key,void *val);
	void del(void *key);

private:
	enum {BLOCK_SIZE = 2};
	int m_size;
	int m_num;
	void **m_ptr;
};
#endif