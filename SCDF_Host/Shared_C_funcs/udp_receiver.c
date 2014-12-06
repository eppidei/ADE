#include "udp_receiver.h"
#include <stdio.h>
#include <string.h>
#include "my_defines.h"
#include "my_typedefs.h"

#if (_OS_==1)

#include <winsock.h>
#include <windows.h>

void SCDF_Init(char* ip_local,char* ip_remote,int port_id,SOCKET* SOCK_sd,struct sockaddr_in *localport_info)
{

	static unsigned int iOptVal_pkt;
	static unsigned int ActualOptVal_pkt;
	static int iOptLen_pkt = sizeof(ActualOptVal_pkt);
	unsigned int rcvTimeOut_pkt = 10; 

    WSADATA w;								/* Used to open Windows connection */
    /* Open windows connection */
    if (WSAStartup(0x0202, &w) != 0)
    {
        printf("SCDF-> Could not create windows connection.\n");
        exit(0);
    }

    /* Open a datagram socket */
    *SOCK_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*SOCK_sd == INVALID_SOCKET)
    {
        printf("SCDF-> Could not create socket on port xxxx.\n");
        WSACleanup();
        exit(0);
		printf("SCDF-> Could not create socket on port xxxx.\n");
    }

    /* Clear out address struct */
    memset(localport_info,0, sizeof(struct sockaddr_in));
    //memset(&remote_006,0, sizeof(struct sockaddr_in));

    /* Fill remote */
//    remote_006.sin_family      = AF_INET;
//    remote_006.sin_addr.s_addr = inet_addr(ip_remote);
//    remote_006.sin_port        = htons(IF_C1_DATA_ANALYSIS_LISTEN_PORT);

    /* Fill local */
    localport_info->sin_family       = AF_INET;
    localport_info->sin_addr.s_addr  = inet_addr(ip_local);
    localport_info->sin_port         = htons(port_id);

    /* Bind local address to socket */
    if (bind(*SOCK_sd,(struct sockaddr *)localport_info,sizeof(struct sockaddr_in)) == -1)
    {
         printf("SCDF-> Could not bind socket on port %d  : %d\n", port_id,WSAGetLastError());
        perror("SCDF-> Could not bind socket on port .\n");
        closesocket(*SOCK_sd);
        WSACleanup();


      exit(0);
    }

    /* Setting reception timeout ( NON_BLOCKING RECV/RECVFROM ) */
	if( rcvTimeOut_pkt>0 )
	{
		setsockopt(*SOCK_sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &rcvTimeOut_pkt, sizeof(rcvTimeOut_pkt));
	}



	/* Setting reception buffer */
	iOptVal_pkt= RECEPTION_MAX_BUFFER_LEN*sizeof(SensorData_T);//100 è per non perdere i pacchetti maybe to fix //10 * sizeof(t_usp_c1_data_msg);
	if ( setsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal_pkt, sizeof(iOptVal_pkt))== SOCKET_ERROR ){

		printf("SCDF-> Error on port %d at setsockopt(): %d\n",port_id, WSAGetLastError());
        WSACleanup();
		printf("SCDF-> Error ");
        exit(0);
	}
	if ( getsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (char*)&ActualOptVal_pkt, &iOptLen_pkt ) == SOCKET_ERROR){
		printf("SCDF-> Error on port %d at getsockopt(): %d\n",port_id, WSAGetLastError());
		WSACleanup();
		printf("SCDF-> Error ");
		exit(0);
	}
	if(iOptVal_pkt != ActualOptVal_pkt)
	{
		printf("SCDF-> Warning on port 5005 at getsockopt(): cannot fit the requested size buffer!");
	}

}

int SCDF_receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,SOCKET* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count)
{
    int ret = 0;
    int socketlen = sizeof(struct sockaddr_in);
    static BOOL stopped= TRUE;
    static int last_byte_received=0;
	//static fd_set          fds;
	struct timeval  tv  = {5,0};

    FD_ZERO(fds);
    FD_SET( *SOCK_sd, fds );

    if( select( *SOCK_sd+1, fds, NULL, NULL, &tv ) < 0 )
    {
        perror("select on sd_006 failed!");
        return ERROR;
    }

    if( FD_ISSET(*SOCK_sd, fds) )
    {

        ret = recvfrom( *SOCK_sd, line, maxsize, 0 , (struct sockaddr *)localport_info, &socketlen);

        if ( (ret>0 && stopped==TRUE) || (ret>0 && ret!=last_byte_received) )
        {
            printf("SCDF(%d)-> %d bytes received, local structurs size is %d bytes\n\n", ntohs(localport_info->sin_port), ret ,sizeof(SensorData_T));
            stopped = FALSE;
        }
        else if (WSAGetLastError()<0)
        {
            printf(" ERROR code from rcvfrom function%d\n",WSAGetLastError());
            stopped = TRUE;
        }
		
		#ifdef VERBOSE_UDP
		ioctlsocket(*SOCK_sd,FIONREAD,sock_buff_count);
		#endif

    }
    else
    {
            printf("SCDF-> timeout socket %d\n",port_id);
			ioctlsocket(*SOCK_sd,FIONREAD,sock_buff_count);

           // bDumpNavigation = true;

    }

    last_byte_received  = ret;
      return ret;
}


#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>


void SCDF_Init(char* ip_local,char* ip_remote,int port_id,int* SOCK_sd,struct sockaddr_in *localport_info)
{

	static unsigned int iOptVal_pkt;
	static unsigned int ActualOptVal_pkt;
	static int iOptLen_pkt = sizeof(ActualOptVal_pkt);
	unsigned int rcvTimeOut_pkt = 10; 

    /*WSADATA w;	*/							/* Used to open Windows connection */
    /* Open windows connection */
 /*   if (WSAStartup(0x0202, &w) != 0)
    {
        printf("SCDF-> Could not create windows connection.\n");
        exit(0);
    }*/

    /* Open a datagram socket */
    *SOCK_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*SOCK_sd == -1)
    {
        printf("SCDF-> Could not create socket on port xxxx.\n, errno %d",errno);
        /*WSACleanup();*/
        exit(0);
		/*printf("SCDF-> Could not create socket on port xxxx.\n");*/
    }

    /* Clear out address struct */
    memset(localport_info,0, sizeof(struct sockaddr_in));
    /*memset(&remote_006,0, sizeof(struct sockaddr_in));*/

    /* Fill remote */
  /* remote_006.sin_family      = AF_INET;
    remote_006.sin_addr.s_addr = inet_addr(ip_remote);
    remote_006.sin_port        = htons(IF_C1_DATA_ANALYSIS_LISTEN_PORT);*/

    /* Fill local */
    localport_info->sin_family       = AF_INET;
    localport_info->sin_addr.s_addr  = inet_addr(ip_local);
    localport_info->sin_port         = htons(port_id);
    
    /*printf("local port init %d\n",localport_info->sin_port);*/

    /* Bind local address to socket */
    if (bind(*SOCK_sd,(struct sockaddr *)localport_info,sizeof(struct sockaddr_in)) == -1)
    {
         printf("SCDF-> Could not bind socket on port %d  : %d\n", port_id,errno);
        /*perror("SCDF-> Could not bind socket on port .\n");*/
        close(*SOCK_sd);
      /*  WSACleanup();*/


      exit(0);
    }

    /* Setting reception timeout ( NON_BLOCKING RECV/RECVFROM ) */
	if( rcvTimeOut_pkt>0 )
	{
		setsockopt(*SOCK_sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &rcvTimeOut_pkt, sizeof(rcvTimeOut_pkt));
	}



	/* Setting reception buffer */
	iOptVal_pkt= RECEPTION_MAX_BUFFER_LEN*sizeof(SensorData_T);/*100 è per non perdere i pacchetti maybe to fix //10 * sizeof(t_usp_c1_data_msg);*/
	if ( setsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal_pkt, sizeof(iOptVal_pkt))== -1 ){

		printf("SCDF-> Error on port %d at setsockopt(): %d\n",port_id, errno);
        /*WSACleanup();*/
		printf("SCDF-> Error ");
        exit(0);
	}
	if ( getsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (char*)&ActualOptVal_pkt, &iOptLen_pkt ) == -1){
		printf("SCDF-> Error on port %d at getsockopt(): %d\n",port_id, errno);
		/*WSACleanup();*/
		printf("SCDF-> Error ");
		exit(0);
	}
	if(iOptVal_pkt != ActualOptVal_pkt)
	{
		printf("SCDF-> Warning on port 5005 at getsockopt(): cannot fit the requested size buffer! desired %u actual %u\n",iOptVal_pkt,ActualOptVal_pkt);
	}

}

int SCDF_receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,int* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count)
{
    int ret = 0,ret_val=0;
    socklen_t socketlen = sizeof(struct sockaddr_in);
    static bool stopped= true;
    static int last_byte_received=0;
	/*static fd_set          fds;*/
	struct timeval  tv  = {5,0};

    FD_ZERO(fds);
    FD_SET( *SOCK_sd, fds );
    
    ret_val = select( *SOCK_sd+1, fds, NULL, NULL, &tv );

    if( ret_val == -1 )
    {
        printf("select on sd_006 failed! %d\n",errno);
        return -1111;
    }
    else if (ret_val == 0)
    {
      printf("select on sd_006 timedout! \n");
    }
    

    if( FD_ISSET(*SOCK_sd, fds) )
    {

        ret = recvfrom( *SOCK_sd, line, maxsize, 0 , (struct sockaddr *)localport_info, &socketlen);

        if ( (ret>0 && stopped==true) || (ret>0 && ret!=last_byte_received) )
        {
            printf("SCDF(%d)-> %d bytes received, local structurs size is %d bytes\n\n", ntohs(localport_info->sin_port), ret ,sizeof(SensorData_T));
            stopped = false;
        }
        else if (ret<=0)
        {
            printf(" ERROR code from rcvfrom function%d\n",errno);
            stopped = true;
        }
		
		#ifdef VERBOSE_UDP
		ioctl(*SOCK_sd,FIONREAD,sock_buff_count);
		#endif

    }
    else
    {
            printf("SCDF-> timeout socket %d\n",port_id);
			ioctl(*SOCK_sd,FIONREAD,sock_buff_count);

           /* bDumpNavigation = true;*/

    }

    last_byte_received  = ret;
      return ret;
}

#endif




void Parse_audio_data(SensorData_T *pkt, char *char_buff, s_float *matarray, s_uint64 *time_id,s_uint32 *n_samples)
{

	unsigned int i=0;
	float temp_f = 0.0;
	s_int32 temp = 0;
	s_uint64 time_id_temp=0;

	s_int32 num_frames=0,num_chann=0;
    s_uint32 n_samples_int ;
	
	
    memcpy(&time_id_temp,&char_buff[8],sizeof(time_id_temp));
    memcpy(&num_frames,&char_buff[16],sizeof(num_frames));
    memcpy(&num_chann,&char_buff[20],sizeof(num_chann));
    
    n_samples_int=num_frames*num_chann;
    *n_samples=n_samples_int;
    
    memcpy((char*)pkt->data,&char_buff[32],(n_samples_int)*4);
    
    *time_id=time_id_temp;


	for (i=0;i<n_samples_int;i++)
	{
		/*temp=( *(int*)&(pkt->data[i]) );
		conversione dei bytes in float
		temp_f=*((float*)(&temp));
		memcpy(&temp_f,&temp,sizeof(temp_f));*/
		matarray[i]=(s_float)pkt->data[i];
	}

}

void Parse_audio_data_double(SensorData_T *pkt, char *char_buff, s_double *matarray, s_uint64 *time_id,s_uint32 *n_samples)
{

	unsigned int i=0;
	float temp_f = 0.0;
	s_int32 temp = 0;
	s_uint64 time_id_temp=0;

	s_int32 num_frames=0,num_chann=0;
    s_uint32 n_samples_int ;
	
	
    memcpy(&time_id_temp,&char_buff[8],sizeof(time_id_temp));
    memcpy(&num_frames,&char_buff[16],sizeof(num_frames));
    memcpy(&num_chann,&char_buff[20],sizeof(num_chann));
    
    n_samples_int=num_frames*num_chann;
    *n_samples=n_samples_int;
    
    memcpy((char*)pkt->data,&char_buff[32],(n_samples_int)*4);
    
    *time_id=time_id_temp;


	for (i=0;i<n_samples_int;i++)
	{
		/*temp=( *(int*)&(pkt->data[i]) );
		conversione dei bytes in float
		temp_f=*((float*)(&temp));
		memcpy(&temp_f,&temp,sizeof(temp_f));*/
		matarray[i]=(s_double)pkt->data[i];
	}

}

