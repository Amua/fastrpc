#ifndef _SVR_NS_HEADER_H_
#define _SVR_NS_HEADER_H_


#include <vector>
#include <errno.h>
#include <string>
#include <sys/time.h>



using namespace std;


/**
* @author feimat@baidu.com
*
* 
* <pre>
* �����д����
* </pre>
**/

typedef enum _NS_RETURN_STATUS {
    _ASVR_NSHEAD_RET_SUCCESS       =   0, ///<��дOK
    _ASVR_NSHEAD_RET_EPARAM        =  -1, ///<����������
    _ASVR_NSHEAD_RET_EBODYLEN      =  -2, ///<�䳤���ݳ���������
    _ASVR_NSHEAD_RET_WRITE         =  -3, ///<д������
    _ASVR_NSHEAD_RET_READ          =  -4, ///<����Ϣ��ʧ�ܣ��������errno
    _ASVR_NSHEAD_RET_READHEAD      =  -5, ///<����Ϣͷʧ��, �������errno
    _ASVR_NSHEAD_RET_WRITEHEAD     =  -6, ///<д��Ϣͷʧ��, �����ǶԷ������ӹر���
    _ASVR_NSHEAD_RET_PEARCLOSE     =  -7, ///<�Զ˹ر�����
    _ASVR_NSHEAD_RET_ETIMEDOUT     =  -8, ///<��д��ʱ
    _ASVR_NSHEAD_RET_EMAGICNUM     =  -9, ///<magic_num��ƥ��
    _ASVR_NSHEAD_RET_UNKNOWN	     =  -10
} enumResStatus;


typedef struct _ns_header
{
    unsigned short id;              // id
    unsigned short version;         // �汾��
    unsigned int log_id;          // (M)��apache������logid���ᴩһ��������������罻��
    char provider[16];    // (M)�ͻ��˱�ʶ������������ʽ����Ʒ��-ģ����������"sp-ui"
    unsigned int retcode;           //magic_num ������Ϊreturn code
    unsigned int flow_no;           //reserved ��������flow_no
    unsigned int body_len;        //(M)head���������ݵ��ܳ���
} CProtoHeader;

typedef struct _ns_header_rs{
    CProtoHeader head;		///
    int retcode;			///<��Ӧ״̬
}CProtoHeaderRes;


#endif

