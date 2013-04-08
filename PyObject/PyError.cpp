/* -*- C++ -*- */

/* Time-stamp: <2012-06-28 17:48:46 Thursday by 骆志辉> */

/**
 * @file   PyError.cpp
 * @author 骆志辉 <camel_flying@163.com>
 * @date   Thu Jun 28 17:45:50 2012
 *
 * @bref 需要多重的分离和包含编译，所以添加编译唯一宏
 *
 */

#ifndef _PYERROR_CPP__
#define _PYERROR_CPP__

#include <Python.h>
#include <object.h>
#include <structmember.h>


/*
 * structure for the Python type.
 */
typedef struct {
    PyObject_HEAD
    char  errorText[4096];
} udt_GalaError;

static PyObject *g_ErrorException = NULL;

/*
 * forward declarations
 */
static void Error_Free(udt_GalaError* self);
static PyObject *Error_Str(udt_GalaError* self);


/*
 * declaration of members.
 */
static PyMemberDef g_GalaErrorMembers[] = {
  { "message", T_STRING_INPLACE, offsetof(udt_GalaError, errorText), READONLY, "exception information" },
  { NULL, 0, 0, 0 , "member define end"}
};

/*
 * declaration of Python type
 */
static PyTypeObject g_GalaErrorType = {
    PyObject_HEAD_INIT(NULL)
    0,                                  // ob_size
    "PyGala.GalaError",                 // tp_name
    sizeof(udt_GalaError),              // tp_basicsize
    0,                                  // tp_itemsize
    (destructor) Error_Free,            // tp_dealloc
    0,                                  // tp_print
    0,                                  // tp_getattr
    0,                                  // tp_setattr
    0,                                  // tp_compare
    0,                                  // tp_repr
    0,                                  // tp_as_number
    0,                                  // tp_as_sequence
    0,                                  // tp_as_mapping
    0,                                  // tp_hash
    0,                                  // tp_call
    (reprfunc)Error_Str,               	// tp_str
    0,                                  // tp_getattro
    0,                                  // tp_setattro
    0,                                  // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                 // tp_flags
    0,                                  // tp_doc
    0,                                  // tp_traverse
    0,                                  // tp_clear
    0,                                  // tp_richcompare
    0,                                  // tp_weaklistoffset
    0,                                  // tp_iter
    0,                                  // tp_iternext
    0,                                  // tp_methods
    g_GalaErrorMembers,                 // tp_members
    0,                                   // tp_getset
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0
};


/*
 * Error_New()
 *	Create a new error object.
 */
static udt_GalaError *Error_New(
    const char *context)                // context in which error occurred
{
	udt_GalaError *error;
    error = PyObject_NEW(udt_GalaError, &g_GalaErrorType);
    if (!error)
        return NULL;
    snprintf(error->errorText ,sizeof(error->errorText),"%s",context);

    return error;
}

/*
 *	utility function to raise exception.
 */
static void RaiseException(const char * title,const char * ctx)
{
    char szMsg[1024];

    udt_GalaError *error;

    snprintf(szMsg, sizeof(szMsg), "%s Error:%s", title, ctx);
    error = Error_New(szMsg);
    PyErr_SetObject(g_ErrorException, (PyObject*)error);
    //Py_DECREF(error);
}


/*
 *	Error_Free()
 *  Deallocate the environment, disconnecting from the database if necessary.
 */
static void Error_Free(
		udt_GalaError *self)                    // error object
{
    self->ob_type->tp_free((PyObject*) self);
}


/*
 *	Error_Str()
 *	Return a string representation of the error variable.
 */
static PyObject *Error_Str(
		udt_GalaError *self)                    // variable to return the string for
{
    return PyString_FromString(self->errorText);
}


#endif /* _PYERROR_CPP__ */
