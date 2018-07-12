// Acceptor.h: interface for the Acceptor class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Server.h"
#include "ace/Asynch_Acceptor.h"

class Acceptor : public ACE_Asynch_Acceptor<Server>  
{
public:
	Acceptor();
	virtual ~Acceptor();
};

#endif 
