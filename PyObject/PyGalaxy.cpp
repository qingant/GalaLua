/* -*- C++ -*- */

/* Time-stamp: <2012-08-28 16:23:35 星期二 by tao> */

/**
 * @file   PyGalaxy.cpp
 * @author OCaml <camel.flying@gmail.com>
 * @date   Mon Apr 23 17:52:22 2012
 *
 * @bref
 *
 */


#include "PyGalaxy.hpp"
//#include "PyAMQ.cpp"
#include "PyConfigure.cpp"
//#include "PyLogger.cpp"
//#include "PyGalaSVC.cpp"
//#include "PyRouter.cpp"
//#include "PyGalaDB.cpp"
/*	GetVersion()
 *	Get the version of the PyGalaxy
 */
static PyObject* PyGetVersion(
   PyObject* self,                     // passthrough argument
   PyObject* args)                     // arguments to function
{
   ( void )args;
   ( void )self;
   return PyString_FromFormat("1.0");
}

/*
 *	用于测试的API
 */
static PyObject* PyTest(
    PyObject* self,                     // passthrough argument
    PyObject* args)                     // arguments to function
{
   ( void )args;
   ( void )self;
   PyObject *tuple;
   if(!PyArg_ParseTuple(args, "O", &tuple))
   {
      return NULL;
   }
//	Py_ssize_t tupleLen = PyTuple_Size(tuple);
   
//	PyObject *pair = PyTuple_GetItem(tuple, tupleLen+3);
   char buf[256]={0};
   snprintf(buf, sizeof(buf), "PyTEST %s %s %d", (char *)__FILE__, (char *)__FUNCTION__, __LINE__);
   RaiseException("PyGalaxy.TEST", buf);
   printf("-------------after RaiseException---------\n");
   
   return NULL;
}

//static PyObject* PyRequestPack(
//    PyObject* self,                     // passthrough argument
//    PyObject* args)                     // arguments to function
//{
//   ( void )self;
//    CPSTR pHead = NULL, content = NULL;
//    int jmp = 0;
//    int hlen = 0;
//    int clen = 0;
//    int id;
//    int timeout = 0;
//    if(!PyArg_ParseTuple(args, "s#iis#|i", &pHead, &hlen, &id, &jmp, &content, &clen, &timeout))
//    {
//        return NULL;
//    }
//    PGALA_REQUEST pReq = (PGALA_REQUEST)pHead;
//
//    pReq->JUMP = jmp;
//    pReq->PRE_JUMP = id;
//    pReq->LEN = clen;
//    pReq->ISASYNC = 1;
//    pReq->ISDEBUG = 0;
//    pReq->TIMEOUT = timeout;
//
//    std::string buf(sizeof(*pReq)+clen-1, 0);
//    CRT_memcpy(&buf[0], (const char*)pReq, sizeof(*pReq));
//    CRT_memcpy(&buf[sizeof(*pReq)-1], content, clen);
//    return Py_BuildValue("s#", buf.c_str(), buf.size());
//}

//static PyObject* PyCFileMQCreator(
//    PyObject* self,                     // passthrough argument
//    PyObject* args)                     // arguments to function
//{
//   ( void )self;
//   char filename[2048]={0};
//   char *pfilename=NULL;
//   UINT Pages;
//   UINT PageSize;
//   UINT NQs;
//   UINT CQs;
//   
//   if(!PyArg_ParseTuple(args, "sIIII", &pfilename, &Pages, &PageSize, &NQs, &CQs))
//   {
//      return NULL;
//   }
//	
//   try
//   {
//	  CRT_strncpy(filename, pfilename, sizeof(filename));
//	  Py_BEGIN_ALLOW_THREADS;
//	  {
//     Galaxy::AMQ::CGalaxyMQCreator _(std::string(filename), std::string("who"), Pages, PageSize, NQs, CQs, 0, 2500, 0, 0);
//         printf("filename=%s Pages=%u PageSize=%u NQs=%u CQs=%u\n", filename, Pages, PageSize, NQs, CQs);
//	  }
//	  Py_END_ALLOW_THREADS;
//   }
//   catch(Galaxy::AMQ::CMQException &e)
//   {
//      RaiseException("PyGalaxy.CFileMQCreator", e.what());
//      return NULL;
//   }
//   catch (...)
//   {
//      char buf[1024]={0};
//      snprintf(buf, sizeof(buf), "failure PyCFileMQCreator %s %s %d", (char *)__FILE__, (char *)__FUNCTION__, __LINE__);
//      RaiseException("PyGalaxy.CFileMQCreator", buf);
//      return NULL;
//   }
//   
//   Py_INCREF(Py_None);
//   return Py_None;	
//}

/*	SetException()
 *	create an exception and set it in the provided dictionary.
 */
static int SetException(
		PyObject *module,               //module object
		PyObject **exception,           //exception to create
		char *name,                     //name of exception
		PyObject *baseException)        //exception to base exception
{
	char buffer[1024];

	snprintf(buffer, sizeof(buffer), "PyGalaxy.%s", name);
	*exception = PyErr_NewException(buffer, baseException, NULL);
	if(!*exception)
	{
		return -1;
	}

	return PyModule_AddObject(module, name, *exception);
}

static PyMethodDef module_methods[] = {
//    {"GetVersion", PyGetVersion, METH_VARARGS, "Get the version of the PyGalaxy.so"},
//    {"TEST", PyTest, METH_VARARGS, "test exceptions"},
//    {"PackRequest", PyRequestPack, METH_VARARGS, "Pack the Request"},
//    {"CFileMQCreator", PyCFileMQCreator, METH_VARARGS, "create a file MQ"},
    {NULL, 0, 0, NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/*declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

#ifdef __cplusplus
extern "C" {
#endif

PyMODINIT_FUNC
initPyGalaxy(void)
{
    PyObject* m;
    m = Py_InitModule3("PyGalaxy", module_methods, "");
    if (m == NULL)
      return;
    //add exception indicator
    if(SetException(m, &g_ErrorException, "Error", PyExc_BaseException) < 0)
    	return;
    
    PyTypeObject *type;
//    type = &AMQ_Type;
//    if (PyType_Ready(&AMQ_Type) < 0)
//      return;
//    PyModule_AddObject(m, "AMQ", (PyObject *)type);
//    Py_XINCREF(type);

    type = &ConfigurePlt_Type;
    if (PyType_Ready(&ConfigurePlt_Type) < 0)
      return;
    PyModule_AddObject(m, "Configure", (PyObject *)type);
    Py_XINCREF(type);

//    type = &CLogger_Type;
//    if (PyType_Ready(type) < 0)
//      return;
//    PyModule_AddObject(m, "CLogger", (PyObject *)type);
//    Py_XINCREF(type);
//
//    type = &GalaSVC_Type;
//    if (PyType_Ready(type) < 0)
//      return;
//    PyModule_AddObject(m, "GalaSVC", (PyObject *)type);
//    Py_XINCREF(type);
//
//    type = &GalaRouter_Type;
//    if (PyType_Ready(type) < 0)
//      return;
//    PyModule_AddObject(m, "Router", (PyObject *)type);
//    Py_XINCREF(type);
//
//    type = &GalaDB_Type;
//    if (PyType_Ready(type) < 0)
//      return;
//    PyModule_AddObject(m, "GalaDB", (PyObject *)type);
//    Py_XINCREF(type);
}

#ifdef __cplusplus
}
#endif
