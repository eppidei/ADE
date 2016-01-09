#ifndef _ADE_UDP_RECEIVER_H
#define _ADE_UDP_RECEIVER_H

#include "headers/ADE_typedefs.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

struct ADE_UDP_RECEIVER_S
{
struct sockaddr_in SocketAddressLocal;
struct sockaddr_in SocketAddressRemote;
int SocketDesc;
ADE_BUFF_DESCRIPTOR_T RecvBuff;

};
#ifdef __cplusplus
    extern "C" {
#endif
/******** Init Methods ************/
ADE_API_RET_T ADE_UdpReceiver_Init (ADE_UDPRECEIVER_T ** dp_UdpSender);
ADE_VOID_T ADE_UdpReceiver_Release(ADE_UDPRECEIVER_T *p_UdpSender);
/************************************/
ADE_API_RET_T ADE_UdpReceiver_CreateSocket(ADE_UDPRECEIVER_T *p_UdpReceiver);
ADE_API_RET_T ADE_UdpReceiver_CloneSocket(ADE_UDPRECEIVER_T *p_UdpReceiver,ADE_UDPRECEIVER_T *p_UdpReceiver2Clone);
ADE_API_RET_T ADE_UdpReceiver_SetRemote(ADE_UDPRECEIVER_T *p_UdpReceiver,char* address,int srcport);
ADE_API_RET_T ADE_UdpReceiver_SetLocal(ADE_UDPRECEIVER_T *p_UdpReceiver,char* address,int dstport);
ADE_API_RET_T ADE_UdpReceiver_SetTimeOut(ADE_UDPRECEIVER_T *p_UdpReceiver,unsigned int timeout);
ADE_API_RET_T ADE_UdpReceiver_SetBuffDim(ADE_UDPRECEIVER_T *p_UdpReceiver,unsigned int buffdim);
ADE_API_RET_T ADE_UdpReceiver_SetNonBlocking(ADE_UDPRECEIVER_T *p_UdpReceiver);
ADE_API_RET_T ADE_UdpReceiver_Recv(ADE_UDPRECEIVER_T *p_UdpReceiver,ssize_t *p_NumBytesRecv);
ADE_API_RET_T ADE_UdpReceiver_Recvfrom(ADE_UDPRECEIVER_T *p_UdpReceiver,ssize_t *p_NumBytesRecv,struct sockaddr *src_addr, socklen_t *addrlen);
ADE_API_RET_T ADE_UdpReceiver_GetDescriptor(ADE_UDPRECEIVER_T *p_UdpReceiver,int *p_SocketDesc);
ADE_API_RET_T ADE_UdpReceiver_SetReusePort(ADE_UDPRECEIVER_T *p_UdpReceiver);
ADE_API_RET_T ADE_UdpReceiver_SetReuseAddress(ADE_UDPRECEIVER_T *p_UdpReceiver);
ADE_API_RET_T ADE_UdpReceiver_SetRemoteAnyIP(ADE_UDPRECEIVER_T *p_UdpReceiver,int srcport);
#ifdef __cplusplus
    }   /* extern "C" */
#endif
#endif // _ADE_UDP_RECEIVER_H
