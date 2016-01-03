#ifndef _ADE_UDP_SENDER_H
#define _ADE_UDP_SENDER_H

#include "headers/ADE_typedefs.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

struct ADE_UDP_SENDER_S
{
struct sockaddr_in SocketAddressLocal;
struct sockaddr_in SocketAddressRemote;
int SocketDesc;
ADE_BUFF_DESCRIPTOR_T SendBuff;
ADE_BUFF_DESCRIPTOR_T ExternalSendBuff;

};
/******** Init Methods ************/
ADE_API_RET_T ADE_UdpSender_Init (ADE_UDPSENDER_T ** dp_UdpSender);
ADE_VOID_T ADE_UdpSender_Release(ADE_UDPSENDER_T *p_UdpSender);
/***********************/
ADE_API_RET_T ADE_UdpSender_CreateSocket(ADE_UDPSENDER_T *p_UdpSender);
ADE_API_RET_T ADE_UdpSender_SetRemote(ADE_UDPSENDER_T *p_UdpSender,char* address,int dstport);
ADE_API_RET_T ADE_UdpSender_SetLocal(ADE_UDPSENDER_T *p_UdpSender,int srcport);
ADE_API_RET_T ADE_UdpSender_SendInternalBuff(ADE_UDPSENDER_T *p_UdpSender,size_t size2send, ssize_t *p_SentSize);
ADE_API_RET_T ADE_UdpSender_SetInternalBuff(ADE_UDPSENDER_T *p_UdpSender,ADE_VOID_T* p_buff, size_t BuffSize);
#endif //_ADE_UDP_SENDER_H
