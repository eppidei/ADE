#ifndef _APPLEMIDI_CTRLCH
#define _APPLEMIDI_CTRLCH
#include "AppleMIDI.h"
#include "headers/ADE_Udp_Sender.h"
#include "headers/ADE_Udp_Receiver.h"
struct APPLEMIDI_CTRLCH_S
{

ADE_UDPSENDER_T *p_Sender;
ADE_UDPRECEIVER_T *p_Receiver;
APPLEMIDI_T AppleMidiRx;
APPLEMIDI_T AppleMidiTx;
};

typedef struct APPLEMIDI_CTRLCH_S APPLEMIDI_CTRLCH_T;

int AppleMidiCtrlCh_Init(APPLEMIDI_CTRLCH_T **dp_ch);
void AppleMidiCtrlCh_ConfigUDPSender(APPLEMIDI_CTRLCH_T *p_ch,char *p_RemoteIp, int DstPort, int SrcPort);
void AppleMidiCtrlCh_ConfigUDPReceiver(APPLEMIDI_CTRLCH_T *p_ch,char *p_RemoteIp, int DstPort, int SrcPort);
void AppleMidiCtrlCh_PrepareAcceptInvitationPacket(APPLEMIDI_T *p_pkt_tx,int IniziatorToken,unsigned int  SenderSource,char *p_name ,int *p_actual_len);
void AppleMidiCtrlCh_ParsePacket(APPLEMIDI_CTRLCH_T *p_ch);

#endif //_ADE_APPLEMIDI_CTRLCH
