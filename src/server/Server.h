// Server.h: interface for the Server class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SERVER_H
#define SERVER_H

#include "ace/asynch_io.h"
#include "../common/Test_def.h"
#include <vector>
#include <tchar.h>

using namespace std;

#define ROLE_COUNT 6

typedef struct {
	TCHAR name[16];
	TCHAR key[16];
	short int PID;
	short int kind;
	short int tx;
	short int ty;
}Role;

static Role role[ROLE_COUNT] = {_T("test"), _T("123456"), 001, 0, 250, 360,
								_T("guest"), _T("guest"), 002, 1, 200, 360,
								_T("admin"), _T("admin"), 003, 0, 300, 360,
								_T("root"), _T("root"), 004, 1, 150, 360,
								_T("hnau"), _T("123456"), 005, 0, 350, 360,
								_T("common"), _T("123456"), 006, 0, 400, 360,
							   };

static vector<PERSONAE> usr;

class Server : public ACE_Service_Handler  
{
public:
	Server();
	virtual ~Server();

	/// This is called after the new connection has been accepted.
	virtual void open (ACE_HANDLE handle,
                     ACE_Message_Block &message_block);

protected:
	/**
	* @name AIO callback handling
	*
	* These methods are called by the framework
	*/
	/// This is called when asynchronous <read> operation from the
	/// socket completes.
	virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);

	/// This is called when an asynchronous <write> to the socket
	/// completes.
	virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);

//	virtual void addresses (const ACE_INET_Addr &remoteAddr,
//                            const ACE_INET_Addr &localAddr);

	void notify_other ();
		
private:
	int initiate_read_stream (void);
	int initiate_write_stream (ACE_Message_Block &mb, size_t nbytes);
	void start_write (ACE_Message_Block *mblk);
	bool proccess_mesage(ACE_Message_Block *mb);

	enum { HEADER_SIZE = sizeof(MsgHead) };   
	ACE_HANDLE handle_;
	ACE_Asynch_Read_Stream rs_;
	ACE_Asynch_Write_Stream ws_;
	
	static vector<Server*> _handles;
	PERSONAE usr_t;
};

#endif 
