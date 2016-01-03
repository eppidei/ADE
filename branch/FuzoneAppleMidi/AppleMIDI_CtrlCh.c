#include "AppleMIDI_CtrlCh.h"
#include <stdlib.h>
#include <string.h>

static AppleMidiCtrlCh_doParsePacket(char *p_buff, APPLEMIDI_T* p_AppleMIDIPkt);


int AppleMidiCtrlCh_Init(APPLEMIDI_CTRLCH_T **dp_ch)
{
    APPLEMIDI_CTRLCH_T *p_this=NULL;

    p_this=calloc(1,sizeof(APPLEMIDI_CTRLCH_T));

    if (p_this!=NULL)
    {
        ADE_UdpSender_Init(&(p_this->p_Sender));
        ADE_UdpReceiver_Init(&(p_this->p_Receiver));
        *dp_ch=p_this;

        return 0;
    }
    else
    {

        return -1;
    }

}
void AppleMidiCtrlCh_ConfigUDPSender(APPLEMIDI_CTRLCH_T *p_ch,char *p_RemoteIp, int DstPort, int SrcPort)
{
ADE_UdpSender_CreateSocket(p_ch->p_Sender);
ADE_UdpSender_SetRemote(p_ch->p_Sender,p_RemoteIp,DstPort);
ADE_UdpSender_SetLocal(p_ch->p_Sender,SrcPort);

}
void AppleMidiCtrlCh_ConfigUDPReceiver(APPLEMIDI_CTRLCH_T *p_ch,char *p_RemoteIp, int DstPort, int SrcPort)
{
ADE_UdpReceiver_CreateSocket(p_ch->p_Receiver);
ADE_UdpReceiver_SetRemote(p_ch->p_Receiver,p_RemoteIp,DstPort);
ADE_UdpReceiver_SetLocal(p_ch->p_Receiver,SrcPort);
ADE_UdpReceiver_SetNonBlocking(p_ch->p_Receiver);
}

void AppleMidiCtrlCh_PrepareAcceptInvitationPacket(APPLEMIDI_T *p_pkt_tx,int IniziatorToken,unsigned int  SenderSource,char *p_name,int *p_actual_len)
{
    p_pkt_tx->header.Signature=-1;
    p_pkt_tx->header.Command=InvitationAccepted;
     p_pkt_tx->Invitation.ProtVersion=2;
     p_pkt_tx->Invitation.IniziatorToken=IniziatorToken;
     p_pkt_tx->Invitation.SenderSource=SenderSource;
     strcpy(p_pkt_tx->Invitation.name,p_name);

     *p_actual_len=4+4+4+4+strlen(p_name);



}

void AppleMidiCtrlCh_ParsePacket(APPLEMIDI_CTRLCH_T *p_ch)
{
    AppleMidiCtrlCh_doParsePacket(p_ch->p_Receiver->RecvBuff.p_Buff,&(p_ch->AppleMidiRx));

}

static AppleMidiCtrlCh_doParsePacket(char *p_buff, APPLEMIDI_T* p_AppleMIDIPkt)
{

    unsigned int offset = 0;
    APPLEMIDI_T * p_int_buff=NULL;

    p_int_buff=(APPLEMIDI_T *)p_buff;
    short Command,Signature;

    Command=p_int_buff->header.Command;
     Signature=p_int_buff->header.Signature;

     p_AppleMIDIPkt->header.Command=Command;
     p_AppleMIDIPkt->header.Signature=Signature;


    if (Command==Invitation || Command==InvitationRejected || Command==InvitationAccepted )
    {

        p_AppleMIDIPkt->Invitation.ProtVersion=p_int_buff->Invitation.ProtVersion;
        p_AppleMIDIPkt->Invitation.IniziatorToken=p_int_buff->Invitation.IniziatorToken;
        p_AppleMIDIPkt->Invitation.SenderSource=p_int_buff->Invitation.SenderSource;
        strcpy(p_AppleMIDIPkt->Invitation.name,p_int_buff->Invitation.name);

    }
    else if (Command==Synchronization)
    {
        p_AppleMIDIPkt->Synchronization.SenderSSRC=p_int_buff->Synchronization.SenderSSRC;
        p_AppleMIDIPkt->Synchronization.Count=p_int_buff->Synchronization.Count;
        p_AppleMIDIPkt->Synchronization.Padding=p_int_buff->Synchronization.Padding;
        p_AppleMIDIPkt->Synchronization.TimeStamp1=p_int_buff->Synchronization.TimeStamp1;
        p_AppleMIDIPkt->Synchronization.TimeStamp2=p_int_buff->Synchronization.TimeStamp2;
        p_AppleMIDIPkt->Synchronization.TimeStamp3=p_int_buff->Synchronization.TimeStamp3;

    }
    else if (Command==ReceiverFeedback)
    {
        p_AppleMIDIPkt->RecvFeedBack.SenderSSRC=p_int_buff->RecvFeedBack.SenderSSRC;
        p_AppleMIDIPkt->RecvFeedBack.SequenceNumber=p_int_buff->RecvFeedBack.SequenceNumber;

    }
    else
    {

        fprintf(stderr, "APPLE COMMAND UNKNOWN !\n");

    }


}


