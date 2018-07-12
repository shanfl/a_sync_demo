// MyTask.h: interface for the MyTask class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYTASK_H
#define MYTASK_H

#include "ace/Task.h"
#include "ace/Synch_Traits.h"
#include "ace/Thread_Semaphore.h"
#include "ace/Proactor.h"

class MyTask : public ACE_Task<ACE_MT_SYNCH>  
{
public:
	MyTask (ACE_Proactor *p ):proactor_(p){};
	MyTask ();
	virtual ~MyTask();

	virtual int svc (void);

	int start (int num_threads);
	int stop  (void);

private:

	ACE_SYNCH_RECURSIVE_MUTEX lock_;
	ACE_Thread_Semaphore sem_;
	ACE_Proactor * proactor_;
};

#endif 
