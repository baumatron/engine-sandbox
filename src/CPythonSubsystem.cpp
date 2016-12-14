#include "CPythonSubsystem.h"
#include "con_main.h"
#include <fstream>
using namespace std;
#include <boost/python.hpp>
using namespace boost::python;
#include "PythonExtentions.h"
CPythonSubsystem Python;

// used to get stdout input
// explained here: http://www.ragestorm.net/tutorial?id=21
PyObject* log_CaptureStdout(PyObject* self, PyObject* pArgs)
{
 char* LogStr = NULL;
 if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) return NULL;

 //printf("%s", LogStr); 
 ccout << string(LogStr);
 // Simply using printf to do the real work. 
 // You could also write it to a .log file or whatever...
 // MessageBox(NULL, LogStr...
 // WriteFile(hFile, LogStr...

 Py_INCREF(Py_None);
 return Py_None;
}

// Notice we have STDERR too.
PyObject* log_CaptureStderr(PyObject* self, PyObject* pArgs)
{
 char* LogStr = NULL;
 if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) return NULL;

 //printf("%s", LogStr);
 ccout << string(LogStr);

 Py_INCREF(Py_None);
 return Py_None;
}

static PyMethodDef logMethods[] = {
 {"CaptureStdout", log_CaptureStdout, METH_VARARGS, "Logs stdout"},
 {"CaptureStderr", log_CaptureStderr, METH_VARARGS, "Logs stderr"},
 {NULL, NULL, 0, NULL}
};
/*
BOOST_PYTHON_MODULE(log)
{
	def("CaptureStdout", log_CaptureStdout, "Logs stdout");
	def("CaptureStderr", log_CaptureStderr, "Logs stderr");
}*/

bool CPythonSubsystem::Initialize()
{
	ccout << "Initializing Python interpereter... ";
	Py_Initialize();
	initinjection();
	//initlog();
	Py_InitModule("log", logMethods);
	// next, set up the logging callback functions
	PyRun_SimpleString(
		"import log\n"
		"import sys\n"
		"class StdoutCatcher:\n"
		"\tdef write(self, str):\n"
		"\t\tlog.CaptureStdout(str)\n"
		"class StderrCatcher:\n"
		"\tdef write(self, str):\n"
		"\t\tlog.CaptureStderr(str)\n"
		"sys.stdout = StdoutCatcher()\n"
		"sys.stderr = StderrCatcher()\n"
		);
	PyRun_SimpleString(
		"import injection\n"
		"Sound = injection.GetSound()\n"
		"Gui = injection.GetGui()\n"
		"Hsi = injection.GetHsi()\n"
		);

	ccout << "done.\n";
	return true;
}


bool CPythonSubsystem::Shutdown()
{
	Py_Finalize();
	return true;
}

void CPythonSubsystem::ExecuteString(string pythonCode)
{
	object main_module(( handle<>(borrowed(PyImport_AddModule("__main__"))) ));

	object main_namespace = main_module.attr("__dict__");

	try
	{
		handle<> ignored((PyRun_String(

			pythonCode.c_str()

			, Py_file_input
			, main_namespace.ptr()
			, main_namespace.ptr())));
	}
	catch(error_already_set)
	{
		PyErr_Print();
	}

//	int five_squared = extract<int>(main_namespace["result"]);
	
}

void CPythonSubsystem::ExecuteFile(string pythonFile)
{
	ifstream file;
	file.open(pythonFile.c_str());
	if(!file.is_open())
		return;

	int filesize = 0;
	file.seekg(0, ios::end);
	filesize = file.tellg();
	file.seekg(0, ios::beg);

	char * fileBuffer = new char[filesize+1];
    memset( fileBuffer, 0, filesize );
	file.read(fileBuffer, filesize);
	fileBuffer[filesize] = '\0';

	string script;
	script.assign(fileBuffer);

	this->ExecuteString(script);

	delete [] fileBuffer;
	file.close();
}



