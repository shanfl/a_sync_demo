// client.cpp: implementation of the client class.
//
//////////////////////////////////////////////////////////////////////

#include "client.h"
#include "ace/os_memory.h"
#include "ace/os_ns_string.h"
#include "ace/os_ns_sys_socket.h"
#include "ace/log_msg.h"
#include "ace/CDR_Stream.h"
#include "ace/proactor.h"
#include "var_def.h"
#include "test.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Client::Client():
//	dlg_(0),
	handle_(ACE_INVALID_HANDLE)
{
	client = this;
}

Client::~Client()
{
	ws_.cancel();
	rs_.cancel();
	if (this->handle_ != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket (this->handle_);
	}
	this->handle_= ACE_INVALID_HANDLE;
}

void
Client::open (ACE_HANDLE handle, ACE_Message_Block &message_block)
{
	handle_ = handle;
	
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
	
	client = 0;
	delete this;
}

int
Client::initiate_write_stream (void)
{
	return 0;
}

void Client::start_write(ACE_Message_Block *mblk)
{
	ws_.write (*mblk, mblk->length ());		
}

int
Client::initiate_read_stream (void)
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
			ACE_TEXT ("(%t) Client , %p\n"),
			ACE_TEXT ("read")),
			-1);
	}


	return 0;
}

void
Client::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
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

void
Client::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
	ACE_Message_Block &mblk = result.message_block();
	if(!result.success() || result.bytes_transferred () == 0)
	{
		//读失败
		ACE_DEBUG ((LM_DEBUG,
				  "completed\n"));

		// We are done: commit suicide.
		connected = FALSE;

		client = 0;
		delete this;

		

		return;
	}
	else if (result.bytes_transferred () < result.bytes_to_read())
	{
		this->rs_.read (mblk, result.bytes_to_read () -
                  result.bytes_transferred ());
	}
	else if( mblk.length() == HEADER_SIZE )
	{
		ACE_DEBUG ((LM_DEBUG,
				  "completed\n"));
		
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
		process_mesage(mb);
		mb->release();
		mblk.release ();

		//发起下一次异步调用
		ACE_NEW_NORETURN(mb, ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE));
		rs_.read (*mb, HEADER_SIZE);
	}   

	return;
}

BOOL Client::process_mesage(ACE_Message_Block *mb)
{
	//假定包正确
	MsgHead *hd = (MsgHead*)mb->rd_ptr();
	
	if(hd->type == MSG_REPLY)
	{
		switch(hd->req_type)
		{
		case REPLY_LOGIN:
			{
				ReplyLogin *msg = (ReplyLogin*)mb->rd_ptr();
				BasicApplication* app = (BasicApplication*)wxApp::GetInstance();
				app->_success = msg->success;
				if (msg->success) {
					_GM->selfPID = msg->PID;
				}
				//TODO: 收到应答后通知正在等待连接者。
				app->_connEvent.signal ();
				break;
			}

		case REPLY_GETOBJ:
			{
				PERSONAE *msg = (PERSONAE*)mb->rd_ptr();
				Sprite obj_t;
				obj_t.genus =0;
				obj_t.action =0;
				obj_t.PID = msg->PID;
				obj_t.kind = msg->kind;
				obj_t.x = obj_t.endx = msg->tx;	
				obj_t.y = obj_t.endy = msg->ty;	
				obj_t.azimuth = msg->azimuth;
				_GM->obj.push_back(obj_t);
				int n = _GM->GetItemByPID (msg->PID);
				if (n == -1 ) return FALSE;
				_GM->SetSprite(n);
				break;
			}
		case REPLY_DIRECT:
			{
				PERSONAE *msg = (PERSONAE*)mb->rd_ptr();
				int n = _GM->GetItemByPID (msg->PID);
				if ( n != -1) {
//					_GM->obj[n].PID = msg->PID;
					_GM->obj[n].azimuth = msg->azimuth;
					_GM->obj[n].endx = msg->tx;
					_GM->obj[n].endy = msg->ty;
					_GM->obj[n].oc = _GM->obj[n].oct - 1;
				}
				break;
			}
		case REPLY_DELOBJ:
			{
				PERSONAE *msg = (PERSONAE*)mb->rd_ptr();
				vector<Sprite>::iterator it;
				for(it = _GM->obj.begin(); it != _GM->obj.end(); ++it) {
					if ((*it).PID == msg->PID) {
						_GM->obj.erase(it);
						break;
					}
				}
				break;
			}
		case REPLY_CHAT:
			{
				RTChat *msg = (RTChat*)mb->rd_ptr();
				
				if (_GM->chat.size() >= 5)
					_GM->chat.pop_back();
				_GM->chat.insert(_GM->chat.begin(), msg->message);

				break;
			}
		}	
		
		return TRUE;
	}
	
	return FALSE;
}
