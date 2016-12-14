
#include "con_main.h"
#include "threads_main.h"
#ifdef WIN32
#include <windows.h>
#endif

Monitor::Monitor()
{
#ifdef WIN32
	mutex = CreateMutex(NULL, false, NULL);
#endif
}

Monitor::~Monitor()	
{
#ifdef WIN32
	if(mutex)
	{
		CloseHandle(mutex);
		mutex = NULL;
	}
#endif
}

void Monitor::mutexOn()
{
#ifdef WIN32
	WaitForSingleObject(mutex, INFINITE);
#endif
}

void Monitor::mutexOff()
{
#ifdef WIN32
	ReleaseMutex(mutex);
#endif
}



// ThreadProcRelay is required for the Thread class 
// it is used because windows won't allow a class member function to be called
#ifdef WIN32
static DWORD WINAPI ThreadProcRelay(Thread* pthis)
{
	return pthis->threadProc();
}
#endif


Thread::Thread():
handle(NULL),
id(0),
running(false)
{
}

Thread::~Thread()
{
	stop();
}

void Thread::start()
{
#ifdef WIN32
	if(handle)
		stop();

	running = true;
	handle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)ThreadProcRelay, this, 0, &id);
#endif
}

void Thread::stop()
{
#ifdef WIN32
	if(handle)
	{
		running = false;
		ccout << "Waiting for thread to stop... ";
		WaitForSingleObject( handle, 5000 );
		ccout << "stopped or timed out.\n";
		CloseHandle(handle);
		handle = NULL;
	}
#endif
}

/*DWORD Thread::threadProc()
{
	return 0;
}*/
