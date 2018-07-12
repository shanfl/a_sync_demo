// MyTask.cpp: implementation of the MyTask class.
//
//////////////////////////////////////////////////////////////////////
#include "MyTask.h"

#include "ace/win32_proactor.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyTask::MyTask():
	lock_ (),
	sem_ ((unsigned int) 0),
	proactor_(0)
{

}

MyTask::~MyTask()
{
}


int
MyTask::start (int num_threads)
{
   if (this->activate (THR_NEW_LWP, num_threads) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT ("%p.\n"),
                       ACE_TEXT ("unable to activate thread pool")),
                      -1);

  for (; num_threads > 0; num_threads--)
    {
      sem_.acquire ();
    }

  return 0;
}


int
MyTask::stop ()
{
  if (this->proactor_ != 0)
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("(%t) Calling End Proactor event loop\n")));

      ACE_Proactor::end_event_loop ();
    }

  if (this->wait () == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p.\n"),
                ACE_TEXT ("unable to stop thread pool")));

  return 0;
}

int
MyTask::svc (void)
{
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("-----------------------------------------------------------------\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("*            Multiple Player Online Game DEMO Server            *\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("*                                                               *\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("* Copyright (c) 2006, by Zhao Dexiang.                          *\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("*                                             zdxster@gmail.com *\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("-----------------------------------------------------------------\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("\nServer has started.\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("\nPress <Enter> key to exit...\n")));

  // signal that we are ready
  sem_.release (1);

  ACE_Proactor::instance ()->proactor_run_event_loop();

//  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("(%t) Task finished\n")));
  ACE_DEBUG ((LM_INFO, ACE_TEXT ("\nThe Program Terminated.\n")));
  return 0;
}
