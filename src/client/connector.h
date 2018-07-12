// connector.h: interface for the connector class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "client.h"
#include "ace/Asynch_connector.h"


class Connector : public ACE_Asynch_Connector<Client>  
{
public:
	Connector();
	virtual ~Connector();

	enum { INITIAL_RETRY_DELAY = 3, MAX_RETRY_DELAY = 30 };

	int  start (const ACE_INET_Addr &addr);
	// Re-establish a connection to the logging server.
	int reconnect (void) { return connect (remote_addr_); }

  // Hook method to detect failure and validate peer before
  // opening handler.
  virtual int validate_connection
    (const ACE_Asynch_Connect::Result& result,
     const ACE_INET_Addr &remote, const ACE_INET_Addr& local);
  
protected:
	// Hook method called on timer expiration - retry connect
	virtual void handle_time_out(const ACE_Time_Value&, const void *);
	// This is called when an outstanding accept completes.
	virtual void handle_connect (const ACE_Asynch_Connect::Result &result);

	virtual Client *make_handler (void);

	// Address at which server listens for connections.
	ACE_INET_Addr remote_addr_;

	// Seconds to wait before trying the next connect
	int retry_delay_;

	BOOL opened;
};

#endif 
