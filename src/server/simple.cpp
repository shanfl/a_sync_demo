// simple.cpp : Defines the entry point for the console application.
//

#include "ace/Asynch_Acceptor.h"
#include "ace/Proactor.h"

#include "Acceptor.h"
#include "mytask.h"

int main(int argc, char* argv[])
{
	// Note: acceptor parameterized by the Server.
	Acceptor acceptor;
	// If passive side

	MyTask task_t(ACE_Proactor::instance());

	if(task_t.start(1) == 0)
	{
		if (acceptor.open (ACE_INET_Addr (PROACTOR_PORT),
											 1024,
											 1) == -1
				)
			return -1;

	}

	getchar();

	task_t.stop();	
	
	return 0;
}
