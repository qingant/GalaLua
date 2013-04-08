/* -*- C++ -*- */

/* Time-stamp: <2012-08-10 16:01:15 星期五 by 骆志辉> */

/**
 * @file   PyConfigure.cpp
 * @author 骆志辉 <camel_flying@163.com>
 * @date   Thu Jun 28 17:50:11 2012
 *
 * @bref
 *
 */

#include "PyConfigure.hpp"

static PyObject* PyConfigurePlt_New(
   PyTypeObject *type,                 // type object
   PyObject *args,                     // arguments
   PyObject *keywordArgs)              // keyword arguments
{
   ( void )args;
   ( void )keywordArgs;
   udt_ConfigurePlt *self;

   self = (udt_ConfigurePlt *)type->tp_alloc(type, 0);
   if (self == NULL) {
      return NULL;
   }
   self->_pConfigurePlt = NULL;

   return (PyObject *)self;
}

static int PyConfigurePlt_Init(
   udt_ConfigurePlt  *self,           // CObject
   PyObject    *args,                 // arguments
   PyObject    *keywordArgs)          // keyword arguments
{
   ( void )keywordArgs;
   try
   {
      PSTR path = NULL;
      char mode;
      if (!PyArg_ParseTuple(args, "s|c", &path,&mode))
         return -1;
      if (mode=='w')
      {
          self->_pConfigurePlt = new ConfigurePlt( path,PROT_READ|PROT_WRITE);
      }
      else
      {
          self->_pConfigurePlt = new ConfigurePlt( path,PROT_READ );
      }
   }
   catch (...)
   {
      printf("Except\n");
      return -1;
   }

   return 0;
}

static void PyConfigurePlt_free(udt_ConfigurePlt* self)
{
   delete self->_pConfigurePlt;
   self->ob_type->tp_free((PyObject *)self);
}

static PyObject *PyGet(PyObject *self, PyObject *args)
{
   PSTR path;
   udt_ConfigurePlt* thiz = (udt_ConfigurePlt *)self;
   std::string ctx;

   if (!PyArg_ParseTuple(args, "s", &path))
      return NULL;
   try
   {
      ctx = thiz->_pConfigurePlt->Get( path );
   }
   catch( const Galaxy::GalaxyRT::CException &e )
   {
      RaiseException("PyGalaxy.Config", e.what( ));
      return NULL;
   }
   catch( ... )
   {
      RaiseException("PyGalaxy.Config", "failure to PyGet, Unexpected exception");
      return NULL;
   }

   return Py_BuildValue( "s", ctx.c_str( ));
}

static PyObject *PyGet_ex(PyObject *self, PyObject *args)
{
   PSTR path = NULL;
   std::pair<std::string, std::string> ctx;
   udt_ConfigurePlt* thiz = (udt_ConfigurePlt *)self;

   if (!PyArg_ParseTuple(args, "s", &path))
      return NULL;
   try
   {
      ctx = thiz->_pConfigurePlt->Get_ex( path );
   }
   catch( const Galaxy::GalaxyRT::CException &e )
   {
      RaiseException("PyGalaxy.Config", e.what( ));
      return NULL;
   }
   catch( ... )
   {
      RaiseException("PyGalaxy.Config", "failure to PyGet, Unexpected exception");
      return NULL;
   }

   return  Py_BuildValue( "(s,s)", ctx.first.c_str(), ctx.second.c_str() );
}

static PyObject *PyPut(PyObject *self, PyObject *args)
{
   PSTR path;
   PSTR value;
   udt_ConfigurePlt* thiz = (udt_ConfigurePlt *)self;

   if (!PyArg_ParseTuple(args, "ss", &path, &value ))
      return NULL;
   try
   {
      thiz->_pConfigurePlt->Put( path, value );
   }
   catch( const Galaxy::GalaxyRT::CException &e )
   {
      RaiseException("PyGalaxy.Config", e.what( ));
      return NULL;
   }
   catch( ... )
   {
      RaiseException("PyGalaxy.Config", "failure to PyGet, Unexpected exception");
      return NULL;
   }

   return Py_None;
}

static PyMethodDef ConfigurePlt_Methods[] = {
   {"Get", (PyCFunction)PyGet, METH_VARARGS, ""},
   {"Get_ex", (PyCFunction)PyGet_ex, METH_VARARGS, ""},
   {"Put", (PyCFunction)PyPut, METH_VARARGS, ""},
   { NULL, 0, 0, 0 }
};

static PyMemberDef ConfigurePlt_Members[] = {
   { NULL, 0, 0, 0, 0 }
};

static PyTypeObject ConfigurePlt_Type = {
   PyObject_HEAD_INIT(NULL)
   0,                                  // ob_size
   "PyGalaxy.Configure",            // tp_name
   sizeof(udt_ConfigurePlt),           // tp_basicsize
   0,                                  // tp_itemsize
   (destructor)PyConfigurePlt_free,    // tp_dealloc
   0,                                  // tp_print
   0,                                  // tp_getattr
   0,                                  // tp_setattr
   0,                                  // tp_compare
   0,                                   // tp_repr
   0,                                  // tp_as_number
   0,                                  // tp_as_sequence
   0,                                  // tp_as_mapping
   0,                                  // tp_hash
   0,                                  // tp_call
   0,                                  // tp_str
   0,                                  // tp_getattro
   0,                                  // tp_setattro
   0,                                  // tp_as_buffer
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   // tp_flags
   0,                                  // tp_doc
   0,                                  // tp_traverse
   0,                                  // tp_clear
   0,                                  // tp_richcompare
   0,                                  // tp_weaklistoffset
   0,                                  // tp_iter
   0,                                  // tp_iternext
   ConfigurePlt_Methods,               // tp_methods
   ConfigurePlt_Members,               // tp_members
   0,                                   // tp_getset
   0,                                  // tp_base
   0,                                  // tp_dict
   0,                                  // tp_descr_get
   0,                                  // tp_descr_set
   0,                                  // tp_dictoffset
   (initproc)PyConfigurePlt_Init,      // tp_init
   0,                                  // tp_alloc
   (newfunc)PyConfigurePlt_New,        // tp_new
   0,                                  // tp_free
   0,                                  // tp_is_gc
   0,                                  // tp_bases
   0,
   0,
   0,
   0,
   0,
   0
};
