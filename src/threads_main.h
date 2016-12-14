#ifndef THREADS_MAIN_H
#define THREADS_MAIN_H

// Thread base class

// to prevent more than 1 thread from accessing the same resource
// at the same time, which has very very bad results: hard to find 
// bugs that appear inconsistently
class DummyMonitor
{
public:
	DummyMonitor(){}
	~DummyMonitor(){}

	void mutexOn(){}
	void mutexOff(){}
};

class Monitor
{
public:
	Monitor();
	~Monitor();

	void mutexOn();
	void mutexOff();
private:
	void* mutex; // void* == HANDLE, but windows is stupid
};


class Thread // base class for a thread
{
public:
	Thread();
	virtual ~Thread();

	void start();
	void stop();
	bool isRunning();

	virtual unsigned long threadProc() = 0; // unsigned long == DWORD

	Monitor monitor;
protected:
	void* handle;			// void* == HANDLE
	unsigned long id;		// unsigned long == DWORD
	volatile bool running;
};


#endif