#include "headers/ADE_Udp_Receiver.h"
#include "headers/ADE_defines.h"
#include "headers/ADE_errors.h"
#include "headers/ADE_Error_Handler.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

ADE_API_RET_T ADE_UdpReceiver_Init (ADE_UDPRECEIVER_T ** dp_UdpReceiver)
{

    ADE_UDPRECEIVER_T *p_this=NULL;
    size_t sendbuffsize =ADE_UDP_RECEIVER_MAX_BUFFER_BYTES;

    p_this=calloc(1,sizeof(ADE_UDPRECEIVER_T));
    ADE_CHECK_MEMALLOC(ADE_CLASS_UDPRECEIVER,ADE_METHOD_Init,p_this);

    p_this->RecvBuff.p_Buff=calloc(1,sendbuffsize);
    p_this->RecvBuff.BuffSize=sendbuffsize;
    ADE_CHECK_MEMALLOC(ADE_CLASS_UDPRECEIVER,ADE_METHOD_Init,p_this->RecvBuff.p_Buff);

    memset(&(p_this->SocketAddressLocal),0,sizeof(p_this->SocketAddressLocal));
    memset(&(p_this->SocketAddressRemote),0,sizeof(p_this->SocketAddressRemote));

    *dp_UdpReceiver=p_this;
    return ADE_RET_SUCCESS;
}


ADE_VOID_T ADE_UdpReceiver_Release(ADE_UDPRECEIVER_T *p_UdpReceiver)
{
    ADE_CHECKNFREE(p_UdpReceiver->RecvBuff.p_Buff);
    ADE_CHECKNFREE(p_UdpReceiver);
}

ADE_API_RET_T ADE_UdpReceiver_CreateSocket(ADE_UDPRECEIVER_T *p_UdpReceiver)
{
    ADE_INT32_T valm1=-1;

    p_UdpReceiver->SocketDesc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPRECEIVER,ADE_METHOD_CreateSocket,p_UdpReceiver->SocketDesc,"%d",valm1);

    return ADE_RET_SUCCESS;

}

ADE_API_RET_T ADE_UdpReceiver_SetRemote(ADE_UDPRECEIVER_T *p_UdpReceiver,char* address,int dstport)
{
    ADE_INT32_T ret;
    ADE_INT32_T valm1=-1;

    p_UdpReceiver->SocketAddressRemote.sin_family = AF_INET;
    p_UdpReceiver->SocketAddressRemote.sin_addr.s_addr = inet_addr(address);
    p_UdpReceiver->SocketAddressRemote.sin_port = htons(dstport);
    ret = connect(p_UdpReceiver->SocketDesc, (const struct sockaddr*)&(p_UdpReceiver->SocketAddressRemote),sizeof(p_UdpReceiver->SocketAddressRemote));
     ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPRECEIVER,ADE_METHOD_Connect,ret,"%d",valm1);

    return ADE_RET_SUCCESS;
}


ADE_API_RET_T ADE_UdpReceiver_SetLocal(ADE_UDPRECEIVER_T *p_UdpReceiver,int srcport)
{
    ADE_INT32_T ret;
    ADE_INT32_T valm1=-1;

    p_UdpReceiver->SocketAddressLocal.sin_family = AF_INET;
    p_UdpReceiver->SocketAddressLocal.sin_addr.s_addr = inet_addr(INADDR_ANY);
    p_UdpReceiver->SocketAddressLocal.sin_port = htons(srcport);
    ret = bind(p_UdpReceiver->SocketDesc, (const struct sockaddr*)&(p_UdpReceiver->SocketAddressLocal),sizeof(p_UdpReceiver->SocketAddressLocal));
    ADE_CHECK_VALUE_NOTEQUAL_ERRNO(ADE_CLASS_UDPRECEIVER,ADE_METHOD_SetLocal,ret,"%d",valm1);

    return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpReceiver_SetTimeOut(ADE_UDPRECEIVER_T *p_UdpReceiver,unsigned int timeout)
{

     setsockopt(p_UdpReceiver->SocketDesc, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpReceiver_SetBuffDim(ADE_UDPRECEIVER_T *p_UdpReceiver,unsigned int buffdim)
{
ADE_INT32_T val0 = 0;

    ADE_CHECK_INTERVAL_G_MIN_LE_MAX(ADE_CLASS_UDPRECEIVER,ADE_METHOD_SetBuffDim,buffdim,"%d",val0,p_UdpReceiver->RecvBuff.BuffSize);

     setsockopt(p_UdpReceiver->SocketDesc, SOL_SOCKET, SO_RCVBUF, (char *) &buffdim, sizeof(buffdim));

     return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpReceiver_SetNonBlocking(ADE_UDPRECEIVER_T *p_UdpReceiver)
{

    int x;
    x=fcntl(p_UdpReceiver->SocketDesc,F_GETFL,0);
    fcntl(p_UdpReceiver->SocketDesc,F_SETFL,x | O_NONBLOCK);

  return ADE_RET_SUCCESS;
}

ADE_API_RET_T ADE_UdpReceiver_Recv(ADE_UDPRECEIVER_T *p_UdpReceiver,ssize_t *p_NumBytesRecv)
{

*p_NumBytesRecv= recv(p_UdpReceiver->SocketDesc, p_UdpReceiver->RecvBuff.p_Buff, p_UdpReceiver->RecvBuff.BuffSize, NULL);

return ADE_RET_SUCCESS;

}
