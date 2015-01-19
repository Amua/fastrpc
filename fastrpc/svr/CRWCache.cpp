#include <string.h>
#include <assert.h>

#include "CRWCache.h"

using namespace std;



CRWCache::CRWCache()
: _mem(NULL), _block_size(0), _data_head(0), _data_len(0), lock(false)
{
}

CRWCache::~CRWCache()
{
	skip(data_len());
    if (lock) {
        pthread_mutex_destroy(&mutex);
    }
	//�ͷ�
}

void CRWCache::SetLock(bool _lock) {
    lock = _lock;
    if (lock) {
        pthread_mutex_init(&mutex, NULL);
    }
}

char* CRWCache::data()
{
	if (_data_len == 0)
		return NULL;

	assert(_data_head < _block_size);
	return _mem + _data_head;
}

unsigned CRWCache::data_len(){return _data_len;}

size_t CRWCache::fixnew(size_t dlen)
{
    size_t rsize = 0;
    if ( dlen<1024 ) rsize = 1024;
    else
    {
        rsize = dlen;
    }
    return rsize;
}

void CRWCache::append_nolock(const char* data, size_t data_len)
{
    // ��һ����Ҫ����ռ䣬_block_size�������Ѿ�����Ŀռ�
	if (_mem == NULL)
	{
        _block_size = fixnew(data_len);
        _mem = new char[_block_size];
        assert(_mem!=NULL);

		memcpy(_mem, data, data_len);

		_data_head = 0;
		_data_len = data_len;
		return;
	}

    // �������ĳ����ܷŵ��ڴ�,��_data_head���ܴ��ڿ�϶���Ȳ���
	if (data_len + _data_head + _data_len <= _block_size)
	{
		memcpy(_mem + _data_head + _data_len, data, data_len);
		_data_len += data_len;
	}
    // �������ĳ����Ѿ������ڴ棬���·��䣬���·��䣬�м�Ͳ����ڿ�϶�ڴ���
	else if (data_len + _data_len > _block_size)
	{
        size_t new_block_size = data_len + _data_len;
        char *mem = new char[new_block_size];
        assert(mem!=NULL);

		memcpy(mem, _mem + _data_head, _data_len);
		memcpy(mem + _data_len, data, data_len);

        if (_mem!=NULL) delete[] _mem;

		_mem = mem;
		_block_size = new_block_size;
		_data_head = 0;
		_data_len += data_len;
	}
    // ��ǰ����ڵ��ڴ��϶�޸�
	else
	{
		memmove(_mem, _mem+_data_head, _data_len);
		memcpy(_mem+_data_len, data, data_len);

		_data_head = 0;
		_data_len += data_len;
	}
}

void CRWCache::append(const char* data, size_t data_len)
{
    // ��һ����Ҫ����ռ䣬_block_size�������Ѿ�����Ŀռ�
	if (_mem == NULL)
	{
        _block_size = fixnew(data_len);
        _mem = new char[_block_size];
        assert(_mem!=NULL);

		memcpy(_mem, data, data_len);

		_data_head = 0;
		_data_len = data_len;
		return;
	}

    // �������ĳ����ܷŵ��ڴ�,��_data_head���ܴ��ڿ�϶���Ȳ���
	if (data_len + _data_head + _data_len <= _block_size)
	{
		memcpy(_mem + _data_head + _data_len, data, data_len);
		_data_len += data_len;
	}
    // �������ĳ����Ѿ������ڴ棬���·��䣬���·��䣬�м�Ͳ����ڿ�϶�ڴ���
	else if (data_len + _data_len > _block_size)
	{
        size_t new_block_size = data_len + _data_len;
        char *mem = new char[new_block_size];
        assert(mem!=NULL);

		memcpy(mem, _mem + _data_head, _data_len);
		memcpy(mem + _data_len, data, data_len);

        if (_mem!=NULL) delete[] _mem;

		_mem = mem;
		_block_size = new_block_size;
		_data_head = 0;
		_data_len += data_len;
	}
    // ��ǰ����ڵ��ڴ��϶�޸�
	else
	{
		memmove(_mem, _mem+_data_head, _data_len);
		memcpy(_mem+_data_len, data, data_len);

		_data_head = 0;
		_data_len += data_len;
	}
}

void CRWCache::skip(unsigned length)
{
    if (_mem == NULL){
        return;
    }
	//skip length�ܴ󣬾��൱���ͷ��ڴ���
	if (length >= _data_len)
	{
        delete[] _mem;
        _mem = NULL;

		_block_size = _data_head = _data_len = 0;
		_data_head = 0;
		_data_len = 0;
	}
	else
	{
		_data_head += length;
		_data_len -= length;
	}
}

