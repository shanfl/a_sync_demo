// connector.cpp: implementation of the connector class.
//
//////////////////////////////////////////////////////////////////////

#include "connector.h"
#include "ace/proactor.h"
#include "var_def.h"
#include "test.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Connector::Connector()
{
	opened = FALSE;
}

Connector::~Connector()
{

}

int
Connector::start (const ACE_INET_Addr& addr)
{
	int rc = 0;

//	ACE_TCHAR str[ACE_MAX_FULLY_QUALIFIED_NAME_LEN + 16];
//  addr.addr_to_string (str, 
//  								ACE_MAX_FULLY_QUALIFIED_NAME_LEN + 16);
//  	
// 	::MessageBox(NULL, (LPCWSTR)str, (LPCWSTR)str, MB_OK);	 	
//
	if (this->open (1, 0, 1) != 0 && !opened)
	{
		ACE_ERROR ((LM_ERROR,
			ACE_LIB_TEXT ("(%t) %p\n"),
			ACE_LIB_TEXT ("Connector::open failed")));
		return rc;
	}

	opened = TRUE;

	if (this->connect (addr) != 0)
	{
		ACE_ERROR ((LM_ERROR,
			ACE_TEXT ("(%t) %p\n"),
			ACE_TEXT ("Connector::connect failed")));
	}


	return rc;
}

int Connector::validate_connection
  (const ACE_Asynch_Connect::Result& result,
   const ACE_INET_Addr &remote, const ACE_INET_Addr&) 
{
	remote_addr_ = remote;
	if (!result.success ()) 
	{
		ACE_Time_Value delay (retry_delay_);
		retry_delay_ *= 2;
		if (retry_delay_ > MAX_RETRY_DELAY)
			retry_delay_ = MAX_RETRY_DELAY;
		ACE_Proactor::instance()->schedule_timer (*this, 0, delay);
		return -1;
	}

	retry_delay_ = INITIAL_RETRY_DELAY;

	return 0;
}

void Connector::handle_time_out(const ACE_Time_Value&, const void *) 
{
	connect (remote_addr_);
}

void
Connector::handle_connect (const ACE_Asynch_Connect::Result &result)
{
	ACE_Asynch_Connector<Client>::handle_connect(result);

	if (!result.success () ||
      result.connect_handle () == ACE_INVALID_HANDLE)
		connected = FALSE;
	else
		connected = TRUE;

	BasicApplication* app = (BasicApplication*)wxApp::GetInstance();
	app->_connEvent.signal ();
}

Client*
Connector::make_handler (void)
{
  // Default behavior
  Client *handler = 0;
  ACE_NEW_RETURN (handler, Client, 0);

  return handler;
}
