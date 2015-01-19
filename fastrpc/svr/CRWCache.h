#ifndef _BAIDU_X_SVR_H_
#define _BAIDU_X_SVR_H_

#include <pthread.h>

/*
    ���ڽ���ependingpool��socket��д������
*/
class CRWCache
{
public:
    CRWCache();
    ~CRWCache();

    char* data();
    unsigned data_len();

    void append(const char* data, size_t data_len);
    void append_nolock(const char* data, size_t data_len);
    void skip(unsigned length);// ���� c.skip(c.data_len())���൱���ͷ��ڴ�

    size_t fixnew(size_t dlen);
    void SetLock(bool _lock=false);

    void extern_lock(){if (lock) {pthread_mutex_lock(&mutex);}}
    void extern_unlock(){if (lock) {pthread_mutex_unlock(&mutex);}}

private:

    char* _mem;
    size_t _block_size; // �Ѿ�����Ŀռ�

    unsigned _data_head; // ͷ����λ�ã����ǿ����ڴ��λ��
    size_t _data_len;    // Ŀǰʹ�õ��ڴ泤��
    pthread_mutex_t mutex; //��
    bool lock;
};

#endif
