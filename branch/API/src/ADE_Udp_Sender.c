#include "headers/ADE_Udp_Sender.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include <string.h>
#include <stdlib.h>



ADE_API_RET_T ADE_UdpSender_Init (ADE_UDPSENDER_T ** dp_UdpSender)
{

    ADE_UDPSENDER_T *p_this=NULL;
    size_t sendbuffsize =ADE_UDP_SENDER_MAX_BUFFER_BYTES;

    p_this=calloc(1,sizeof(ADE_UDPSENDER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_UDPSENDER,ADE_METHOD_Init,p_this);

    p_this->SendBuff.p_Buff=calloc(1,sendbuffsize);
    p_this->SendBuff.BuffSize=sendbuffsize;
    ADE_CHECK_MEMALLOC(ADE_CLASS_UDPSENDER,ADE_METHOD_Init,p_this->SendBuff.p_Buff);

    memset(&(p_this->SocketAddressLocal),0,sizeof(p_this->SocketAddressLocal));
    memset(&(p_this->SocketAddressRemote),0,sizeof(p_this->SocketAddressRemote));

    *dp_UdpSender=p_this;
    return ADE_RET_SUCCESS;
}

ADE_VOID_T ADE_UdpSender_Release(ADE_UDPSENDER_T *p_UdpSender)
{
    ADE_CHECKNFREE(p_UdpSender->SendBuff.p_Buff);
    ADE_CHECKNFREE(p_UdpSender);
}

ADE_API_RET_T ADE_UdpSender_CreateSocket(ADE_UDPSENDER_T *p_UdpSender)
{
    ADE_INT32_T valm1=-1;

    p_UdpSender->SocketDesc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPSENDER,ADE_METHOD_CreateSocket,p_UdpSender->SocketDesc,"%d",valm1);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_UdpSender_SetRemote(ADE_UDPSENDER_T *p_UdpSender,char* address,int dstport)
{
    ADE_INT32_T ret;
    ADE_INT32_T valm1=-1;

    p_UdpSender->SocketAddressRemote.sin_family = AF_INET;
    p_UdpSender->SocketAddressRemote.sin_addr.s_addr = inet_addr(address);
    p_UdpSender->SocketAddressRemote.sin_port = htons(dstport);
    ret = connect(p_UdpSender->SocketDesc, (const struct sockaddr*)&(p_UdpSender->SocketAddressRemote),sizeof(p_UdpSender->SocketAddressRemote));
     ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPSENDER,ADE_METHOD_Connect,ret,"%d",valm1);

    return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_UdpSender_SetLocal(ADE_UDPSENDER_T *p_UdpSender,int srcport)
{
    ADE_INT32_T ret;
    ADE_INT32_T valm1=-1;

    p_UdpSender->SocketAddressLocal.sin_family = AF_INET;
    p_UdpSender->SocketAddressLocal.sin_addr.s_addr = inet_addr(INADDR_ANY);
    p_UdpSender->SocketAddressLocal.sin_port = htons(srcport);
    ret = bind(p_UdpSender->SocketDesc, (const struct sockaddr*)&(p_UdpSender->SocketAddressLocal),sizeof(p_UdpSender->SocketAddressLocal));
    ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPSENDER,ADE_METHOD_SetLocal,ret,"%d",valm1);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpSender_SendInternalBuff(ADE_UDPSENDER_T *p_UdpSender,size_t size2send, ssize_t *p_SentSize)
{
    ADE_INT32_T valm1=-1;

    ADE_CHECK_INPUTPOINTER(ADE_CLASS_UDPSENDER,ADE_METHOD_SendInternalBuff,p_UdpSender->SendBuff.p_Buff);
    *p_SentSize =  send(p_UdpSender->SocketDesc, p_UdpSender->SendBuff.p_Buff, size2send, NULL);//int flags);
    ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPSENDER,ADE_METHOD_SendInternalBuff,*p_SentSize,"%d",valm1);

return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpSender_SetInternalBuff(ADE_UDPSENDER_T *p_UdpSender,ADE_VOID_T* p_buff, size_t BuffSize)
{

    ADE_INT32_T val0=0;

    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_UDPSENDER,ADE_METHOD_SetInternalBuff,BuffSize,"%d",val0,p_UdpSender->SendBuff.BuffSize);

    memcpy(p_UdpSender->SendBuff.p_Buff,p_buff,BuffSize);

    return ADE_RET_SUCCESS;

}

