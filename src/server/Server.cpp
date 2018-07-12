// Server.cpp: implementation of the Server class.
//
//////////////////////////////////////////////////////////////////////

#include "Server.h"

#include "ace/os_memory.h"
#include "ace/os_ns_stdio.h"
#include "ace/os_ns_sys_socket.h"
#include "ace/log_msg.h"
#include "ace/message_block.h"
#include "ace/INET_Addr.h"

#include <algorithm>

vector<Server*> Server::_handles;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Server::Server():
	handle_(ACE_INVALID_HANDLE)
{

}

Server::~Server()
{
	if (this->handle_ != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket (this->handle_);
	}
	this->handle_= ACE_INVALID_HANDLE;

	// remove from the handles vector.
	_handles.erase (find (_handles.begin (), _handles.end (), this));
	
// TODO:
	notify_other ();
}

void
Server::open (ACE_HANDLE handle, ACE_Message_Block &)
{
	handle_ = handle;
	
	_handles.push_back (this);
	
	// Open ACE_Asynch_Write_Stream
	if (this->ws_.open (*this, this->handle_) == -1)
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("(%t) %p\n"),
			ACE_TEXT ("Client::ACE_Asynch_Write_Stream::open")));

	// Open ACE_Asynch_Read_Stream
	else if (this->rs_.open (*this, this->handle_) == -1)
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("(%t) %p\n"),
			ACE_TEXT ("Client::ACE_Asynch_Read_Stream::open")));

	else if (this->initiate_read_stream () != -1)
	{
		return;
	}

	delete this;
}

int
Server::initiate_read_stream (void)
{
	if (this->handle_ == ACE_INVALID_HANDLE)
		return -1;

	// We allocate +1 only for proper printing - we can just set the last byte
	// to '\0' before printing out
	ACE_Message_Block *mb;

	ACE_NEW_RETURN (mb, ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE), -1);

	// Inititiate read
	if (this->rs_.read (*mb, HEADER_SIZE) == -1)
	{
		mb->release ();
		mb = 0;

		ACE_ERROR_RETURN ((LM_ERROR,
			ACE_TEXT ("(%t) Server , %p\n"),
			ACE_TEXT ("read")),
			-1);
	}

	return 0;
}

int
Server::initiate_write_stream (ACE_Message_Block &mb, size_t nbytes)
{
	if (this->handle_ == ACE_INVALID_HANDLE)
		return -1;
   return 0;
}

void
Server::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
	ACE_Message_Block &mblk = result.message_block();
	if(!result.success() || result.bytes_transferred () == 0)
	{
		delete this;
		return;
	}
	//else if (result.bytes_transferred () < result.bytes_to_read())
	//{
	//	this->rs_.read (mblk, result.bytes_to_read () -
 //                 result.bytes_transferred ());
	//}
	else if( mblk.length() == HEADER_SIZE )
	{
		
		MsgHead *hd = (MsgHead*)mblk.rd_ptr();
		ULONG length = hd->length;

		mblk.size (length + HEADER_SIZE);
		rs_.read (mblk, length);
	}
	else
	{
		//校验包的正确性

	
		//处理数据

		ACE_Message_Block *mb = 0;
		ACE_NEW_NORETURN(mb, ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE));
		mb = mblk.clone();
		proccess_mesage(mb);
		mb->release();
		mblk.release ();

		//发起下一次异步调用
		ACE_NEW_NORETURN(mb, ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE));
		rs_.read (*mb, HEADER_SIZE);
	}   

	return;
}

void
Server::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
{
	ACE_Message_Block &mblk = result.message_block();
	if (!result.success () || result.bytes_transferred () == 0) 
	{
		mblk.release();
		//发送失败
	}
	else if(result.bytes_transferred() < result.bytes_to_write())
	{
		//部分发送
		this->ws_.write (mblk, result.bytes_to_write() -
                  result.bytes_transferred ());
	}
	else
	{
		//发送成功
		
		mblk.release ();
	}
	
}

bool Server::proccess_mesage(ACE_Message_Block *mb)
{
	//假定包正确
	MsgHead *hd = (MsgHead*)mb->rd_ptr();
	
	if(hd->type == MSG_REQUEST)
	{
		switch(hd->req_type)
		{
		case REQ_LOGIN:
			{
				ReqLogin *msg = (ReqLogin*)mb->rd_ptr();
//				ACE_DEBUG ((LM_DEBUG, "user %S logined.\n", msg->name));
//				_tprintf (_T("user is: %s .\n"), role[0].name);

				ReplyLogin repy;

				repy.hd.identifier = hd->identifier + 1;
				repy.hd.length = sizeof(ReplyLogin) - sizeof(MsgHead);
				repy.hd.req_type = REPLY_LOGIN;
				repy.hd.retry_times = 0;
				repy.hd.type = MSG_REPLY;
				repy.success = false;

				// 账号和密码校验
//				PERSONAE usr_t;
				for (int i=0; i<ROLE_COUNT; i++) {
					if (_tcscmp(msg->name, role[i].name) == 0 
							&& _tcscmp(msg->key, role[i].key) == 0) {

						vector<PERSONAE>::iterator it;
						for(it = usr.begin(); it != usr.end(); ++it) {
							if ((*it).PID == role[i].PID) {
								goto label;
							}
						}

						_tprintf (_T("> user <%s> logined.\n"), msg->name);
						repy.success = true;
						repy.PID = role[i].PID;
						
						usr_t.PID = role[i].PID;
						usr_t.kind = role[i].kind;
						usr_t.tx = role[i].tx;
						usr_t.ty = role[i].ty;
						usr_t.azimuth = 7;
						usr.push_back(usr_t);
					}
				}
label:
				ACE_Message_Block *mb = 0;
				ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&repy, sizeof(ReplyLogin)));
				mb->wr_ptr(sizeof(ReplyLogin));
				start_write(mb);

				//TODO: 所有的客户端获得新登录的人物
				if (repy.success) {
					usr_t.hd.identifier = hd->identifier + 2;
					usr_t.hd.length = sizeof(PERSONAE) - sizeof(MsgHead);
					usr_t.hd.req_type = REPLY_GETOBJ;
					usr_t.hd.retry_times = 0;
					usr_t.hd.type = MSG_REPLY;
				
					vector<Server*>::const_iterator it;
					for(it = _handles.begin(); it != _handles.end(); ++it) {
						ACE_Message_Block *mb = 0;
						ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&usr_t, sizeof(PERSONAE)));
						mb->wr_ptr(sizeof(PERSONAE));
						(*it)->start_write(mb);
					}
				}
				
				//TODO: 获得其他的人物
				PERSONAE repy_t;
				repy_t.hd.identifier = hd->identifier + 3;
				repy_t.hd.length = sizeof(PERSONAE) - sizeof(MsgHead);
				repy_t.hd.req_type = REPLY_GETOBJ;
				repy_t.hd.retry_times = 0;
				repy_t.hd.type = MSG_REPLY;
				
				vector<PERSONAE>::const_iterator iter;
				for (iter = usr.begin(); iter != usr.end(); ++iter) {
					if ((*iter).PID == usr_t.PID) continue;
					repy_t.PID = (*iter).PID;
					repy_t.kind = (*iter).kind;
					repy_t.tx = (*iter).tx;
					repy_t.ty = (*iter).ty;
					repy_t.azimuth = (*iter).azimuth;
					ACE_Message_Block *mb = 0;
					ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&repy_t, sizeof(PERSONAE)));
					mb->wr_ptr(sizeof(PERSONAE));
					start_write(mb);
				}

				break;
			}
		case REQ_DIRECT:
			{
				PERSONAE *msg = (PERSONAE*)mb->rd_ptr();

				// 通知服务器变更
				vector<PERSONAE>::iterator iter;
				for (iter = usr.begin(); iter != usr.end(); ++iter) {
					if ((*iter).PID == msg->PID) {
						iter->tx = msg->tx;
						iter->ty = msg->ty;
						iter->azimuth = msg->azimuth;
						break;
					}
				}

				// 通知各客户端变更刷新
				PERSONAE repy;
				repy.hd.identifier = hd->identifier + 5;
				repy.hd.length = sizeof(PERSONAE) - sizeof(MsgHead);
				repy.hd.req_type = REPLY_DIRECT;
				repy.hd.retry_times = 0;
				repy.hd.type = MSG_REPLY;
				
				repy.PID = msg->PID;
				repy.kind = msg->kind;
				repy.tx = msg->tx;
				repy.ty = msg->ty;
				repy.azimuth = msg->azimuth;

				vector<Server*>::const_iterator it;
				for(it = _handles.begin(); it != _handles.end(); ++it) {
					ACE_Message_Block *mb = 0;
					ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&repy, sizeof(PERSONAE)));
					mb->wr_ptr(sizeof(PERSONAE));
					(*it)->start_write(mb);
				}

				break;
			}
		case REQ_CHAT:
			{
				RTChat *msg = (RTChat*)mb->rd_ptr();
				
				RTChat repy;

				repy.hd.identifier = hd->identifier + 1;
				repy.hd.length = sizeof(RTChat) - sizeof(MsgHead);
				repy.hd.req_type = REPLY_CHAT;
				repy.hd.retry_times = 0;
				repy.hd.type = MSG_REPLY;
				_tcscpy(repy.message, msg->message);

				vector<Server*>::const_iterator it;
				for(it = _handles.begin(); it != _handles.end(); ++it) {
					ACE_Message_Block *mb = 0;
					ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&repy, sizeof(RTChat)));
					mb->wr_ptr(sizeof(RTChat));
					(*it)->start_write(mb);
				}

				break;
			}
		}	
		
		return TRUE;
	}
	
	return FALSE;
}

void Server::start_write(ACE_Message_Block *mblk)
{
	ws_.write (*mblk, mblk->length ());		
}

//void 	Server::addresses (const ACE_INET_Addr &remoteAddr,
//                            			   const ACE_INET_Addr &localAddr)
//{
//	
//	ACE_TCHAR str[ACE_MAX_FULLY_QUALIFIED_NAME_LEN + 16];
//  remoteAddr.addr_to_string (str, 
//  								ACE_MAX_FULLY_QUALIFIED_NAME_LEN + 16);
//  	
// 	printf ("connected: %s\n", str);	 	
//}

void Server::notify_other ()
{
//	_tprintf (_T("> %d is logout.\n"), usr_t.PID);

	PERSONAE repy;
	repy.hd.length = sizeof(PERSONAE) - sizeof(MsgHead);
	repy.hd.req_type = REPLY_DELOBJ;
	repy.hd.retry_times = 0;
	repy.hd.type = MSG_REPLY;
	repy.hd.identifier = usr_t.hd.identifier;
				
	repy.PID = usr_t.PID;

	// 首先从服务器中删除该人物
	vector<PERSONAE>::iterator iter;
	for(iter = usr.begin(); iter != usr.end(); ++iter) {
		if ((*iter).PID == usr_t.PID) {
			usr.erase(iter);
			break;
		}
	}

	// 通知其它各客户端删除该人物
	vector<Server*>::const_iterator it;
	for(it = _handles.begin(); it != _handles.end(); ++it) {
		ACE_Message_Block *mb = 0;
		ACE_NEW_NORETURN(mb, ACE_Message_Block((char*)&repy, sizeof(PERSONAE)));
		mb->wr_ptr(sizeof(PERSONAE));
		(*it)->start_write(mb);
	}
}
