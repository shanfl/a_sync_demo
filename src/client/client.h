// client.h: interface for the client class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CLIENT_H
#define CLIENT_H

#include "ace/asynch_io.h"
#include "ace/unbounded_set.h"
#include "../common/Test_def.h"

class Client : public ACE_Service_Handler  
{
public:
	Client();
	virtual ~Client();

	enum { WAIT_REPLY_TIME = 10, RETRY_TIMES = 3};
	
	/// This is called after the new connection has been established.
	virtual void open (ACE_HANDLE handle,
					 ACE_Message_Block &message_block);

	virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
	// This is called when asynchronous reads from the socket complete

	virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);
	// This is called when asynchronous writes from the socket complete
	void start_write (ACE_Message_Block *mblk);


private:
	int initiate_read_stream (void);
	int initiate_write_stream (void);
	BOOL process_mesage(ACE_Message_Block *mb);

protected:
	enum { HEADER_SIZE = sizeof(MsgHead) };   
	ACE_HANDLE handle_;
	ACE_Asynch_Read_Stream rs_;
	ACE_Asynch_Write_Stream ws_;

};

#endif 
