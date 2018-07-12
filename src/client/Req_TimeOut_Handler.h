// Req_TimeOut_Handler.h: interface for the Req_TimeOut_Handler class.
//
//////////////////////////////////////////////////////////////////////

#ifndef REQ_TIMEOUT_HANDLER_H
#define REQ_TIMEOUT_HANDLER_H

#include "ace/asynch_io.h"

class Req_TimeOut_Handler : public ACE_Handler  
{
public:
	Req_TimeOut_Handler();
	virtual ~Req_TimeOut_Handler();

	virtual void handle_time_out (const ACE_Time_Value &tv, const void *act = 0);

	ACE_Message_Block *mblk;	
};

#endif 
