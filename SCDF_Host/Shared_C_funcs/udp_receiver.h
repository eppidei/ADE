#ifndef _UDP_RECEIVER_H
#define _UDP_RECEIVER_H

#include "my_typedefs.h"
#include "my_defines.h"



#ifdef __cplusplus
extern "C"
{
#endif
#if (_OS_==1)
void SCDF_Init(char* ip_local,char* ip_remote,int port_id,SOCKET* SOCK_sd,struct sockaddr_in *localport_info);
int SCDF_receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,SOCKET* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count);
#else
#include <sys/types.h>
#include <sys/socket.h>
void SCDF_Init(char* ip_local,char* ip_remote,int port_id,int* SOCK_sd,struct sockaddr_in *localport_info);
int SCDF_receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,int* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count);
#endif
void Parse_audio_data(SensorData_T *pkt, char *char_buff, s_float *matarray, s_uint64 *time_id,s_uint32 *n_samples);
void Parse_audio_data_double(SensorData_T *pkt, char *char_buff, s_double *matarray, s_uint64 *time_id,s_uint32 *n_samples);
#ifdef __cplusplus
}
#endif


#endif
