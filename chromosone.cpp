
#include <Python.h>
#include <structmember.h>

#include <vector>

#include <cstring>
#include <stdlib.h>     /* rand */

/********************************************************************
 * 
 * C defintion for Python class
 * 
********************************************************************/

typedef struct {
  PyObject_HEAD // no semicolon
  size_t len;
  size_t current;
  u_int8_t *data;
} Chromosone ;


/*
 * A helper method to turn the bits into an integer. This
 * is not a public method
 */
uint64_t as_long( Chromosone *a ) {  
  uint64_t val = 0L ;
  uint8_t *data = a->data + a->len - 1 ;
  for( uint64_t mask = 1L ; data >= a->data ; mask<<=1, --data ) {
    if( *data ) {
      val += mask ;
    }
  }
  return val ;
}

/*
 * Given an instance of a Chromosone, set the data to the value
 * of the given data.
 */
void set_long( Chromosone *self, uint64_t x, size_t len ) {
  delete self->data ;
  self->data = new uint8_t[len] ;
  self->len = len ;
  // self->current = 0 ;
  uint64_t mask = 1L << (len - 1) ;

  for( size_t i=0 ; i<len ; ++i, mask >>= 1 ) {
    self->data[i] = ( x & mask ) ? 1 : 0 ;
  }
}

/*
 * Create a new instance of a Chromosone (or a subtype thereof) and
 * initialize its data & size to the values given.
 */
Chromosone *from_long( PyTypeObject *type, uint64_t x, size_t len ) {
  Chromosone *self = (Chromosone *)type->tp_alloc( type, 0 ) ;
  set_long( self, x, len ) ;
  return self ;
}

/********************************************************************
*
* Allocation/deallocation & __init__ definitions 
*
********************************************************************/

static void Chromosone_dealloc(Chromosone *self) {
  delete self->data ;
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Chromosone_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  Chromosone *self;

  self = (Chromosone *)type->tp_alloc(type, 0);
  self->data = NULL ;
  self->len = 0 ;
  self->current = 0 ;
  return (PyObject *)self;
}
 
static int Chromosone_init(Chromosone *self, PyObject *args, PyObject *kwds) {
  int argc = PyTuple_GET_SIZE( args ) ;

  if( argc > 1 ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Constructor accepts a single argument" ) ;
    PyGILState_Release(gstate);
    return 1 ;
  }
  PyObject *arg = PyTuple_GET_ITEM( args, 0 ) ;

  if( PyLong_Check( arg ) ) {
    long l = PyLong_AsLong( arg ) ;
    set_long( self, l, CHAR_BIT*SIZEOF_LONG ) ;
  } else if( PyUnicode_Check( arg ) ) {
    long l = 0L ;
    long mask = 1L ;
    size_t len = PyUnicode_GET_SIZE( arg ) ;   

    Py_UNICODE *s = PyUnicode_AS_UNICODE( arg ) ;

    for( int i=len - 1 ; i>=0 ; --i ) {
        if( s[i] != '0' ) l+= mask ;
        mask <<= 1 ;
    }
    set_long( self, l, len ) ;
  } else if( PyBytes_Check( arg ) ) {
    long l = 0L ; 
    long mask = 1L ;
    size_t len = PyBytes_GET_SIZE( arg ) ;
    char *s = PyBytes_AsString( arg ) ;

    for( int i=len - 1 ; i>=0 ; --i ) {
        if( s[i] != '0' ) l+= mask ;
        mask <<= 1 ;
    }
    set_long( self, l, len ) ;
  } else if( PyObject_TypeCheck( arg, Py_TYPE(self) ) ) {
    Chromosone *other = (Chromosone*)arg ;
    long l = as_long( other ) ;
    set_long( self, l, other->len ) ;
  } else {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Constructor has invalid argument type" ) ;
    PyGILState_Release(gstate);
    return 1 ;
  }
  return 0;
}

/********************************************************************
*
* Member attributes exposed from C definition
*
********************************************************************/


static PyMemberDef Chromosone_members[] = {
    { "capacity", T_PYSSIZET, offsetof(Chromosone, len) , READONLY, "The maximum capacity of the buffer" },
    {NULL, 0, 0, 0, NULL}
};



/********************************************************************
*
* Printing type functions
*
********************************************************************/

static PyObject *Chromosone_repr( Chromosone *self ) {
  uint64_t l = as_long( self ) ;
  uint64_t mask = 1L << ( self->len - 1 ) ;
  char rc[ self-> len + 1] ;

  for( size_t i=0 ; i<self->len ; i++, mask >>= 1L ) {
    rc[i] = ( l & mask ) ? '1' : '0' ;
  }
  rc[self->len] = 0 ;

  return Py_BuildValue( "s", rc ) ;
};


/********************************************************************
 * 
 * Mapping Methods
 * 
 * Used to implement a dictionary
 * 
 * __len__        len(a)
 * __getitem__    a[x]
 * __setitem__    a[x] = y
 *  
 ********************************************************************/
static int Chromosone_len(Chromosone *self) {
  return self->len ;
}

static PyObject* Chromosone_getitem(Chromosone *self, PyObject *ix ) {
  size_t index = PyLong_AsSize_t( ix ) ;
  if( index >= self->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_IndexError, "Out of bounds!" ) ;
    PyGILState_Release(gstate);
    return 0 ;
  }
  
  uint64_t l = as_long( self ) ;
  uint64_t mask = 1L << ( self->len - index - 1 ) ;

  return PyLong_FromLong( ( l & mask ) ? 1 : 0 ) ;
}

static int Chromosone_setitem(Chromosone *self, PyObject *ix, PyObject *val ) {
  size_t index = PyLong_AsSize_t( ix ) ;
  if( index >= self->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_IndexError, "Out of bounds!" ) ;
    PyGILState_Release(gstate);
    return 0 ;
  }
  int v = PyLong_AsLong( val ) ;
  // self->data[index] = v ;
  uint64_t l = as_long( self ) ;
  uint64_t mask = 1L << ( self->len - index - 1 ) ;
  if( v == 0 ) {
    l &= ~mask ;
  } else {
    l |= mask ;
  }
  set_long( self, l, self->len ) ;
  return 0 ;
}

static PyMappingMethods Chromosone_mappings = {
  (lenfunc)Chromosone_len,
  (binaryfunc)Chromosone_getitem,
  (objobjargproc)Chromosone_setitem
};

/********************************************************************
 *
 * Regular Methods
 * 
 ********************************************************************/

static PyObject *mutate( Chromosone *self, PyObject *args ) {
  double probability ;

  int argc = PyTuple_GET_SIZE( args ) ;
  if( argc == 0 ) {
    probability = 0.05 ;
  } else if ( !PyArg_ParseTuple(args, "d", &probability)) {
    Py_RETURN_NONE ;
  }
  if( probability > 0 ) {
    u_int64_t mask = 0 ;
    for( size_t i=0 ; i<self->len ; ++i ) {
      double r = drand48() ;
      if( r < probability ) {
        mask |= 1 ;
      }
      mask <<= 1 ;
    }
    u_int64_t l = as_long( self ) ;
    l ^= mask ;
    set_long(  self, l, self->len ) ;
  }
  Py_RETURN_NONE ;
}


static PyObject *countOnes( Chromosone *self ) {
  u_int64_t l = as_long( self ) ;

  long rc = 0 ;
  for( rc = 0; l > 0; ++rc) {
    l &= l - 1;
  }
  return PyLong_FromLong( rc ) ;
}


static PyObject *countZeros( Chromosone *self ) {
  u_int64_t l = as_long( self ) ;

  long rc = 0 ;
  for( rc = 0; l > 0; ++rc) {
    l &= l - 1;
  }
  return PyLong_FromLong( self->len - rc ) ;
}


static PyObject *new_random( PyTypeObject *type, PyObject *args, PyObject *kwds ) {

  int length = -1 ;
  // int argc = PyTuple_GET_SIZE( args ) ;
  PyObject *arg = PyTuple_GET_ITEM( args, 0 ) ;

  if( PyLong_Check( arg ) ) {
    length = PyLong_AsLong( arg ) ;
  } else {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Random needs 1 numeric argument." ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }

  uint64_t mask = ( 1L << length ) - 1L ;
  uint64_t l = mask & random() ;

  Chromosone *self = (Chromosone *)type->tp_alloc( type, 0 ) ;
  set_long( self, l, length ) ;
  return (PyObject*)self ;
}


static PyObject *from_parents( PyTypeObject *type, PyObject *args ) {

  int argc = PyTuple_GET_SIZE( args ) ;
  std::vector<bool> tmp_data ;

  PyObject *arg1 = PyTuple_GET_ITEM( args, 0 ) ;
  PyObject *arg2 = PyTuple_GET_ITEM( args, 1 ) ;

  if( argc != 2 ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Require 2 arguments of Chromosone types." ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }
  if( !PyObject_TypeCheck( arg1, type ) || !PyObject_TypeCheck( arg2, type ) ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Require 2 arguments of Chromosone types." ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }

  Chromosone *p1 = (Chromosone*)arg1 ;
  Chromosone *p2 = (Chromosone*)arg2 ;

  if( p1->len != p2->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_Exception, "Require 2 arguments of Chromosone types, of the same size." ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }
  Chromosone *self = (Chromosone *)type->tp_alloc( type, 0 ) ;

  u_int64_t l1 = as_long( p1 ) ;
  u_int64_t l2 = as_long( p2 ) ;
  u_int64_t mask = 1L ;
  u_int64_t x = 0 ;

  for( size_t i=0 ; i<p1->len ; ++i, mask <<= 1 ) {
    x |= ( mask & ( drand48() < 0.5 ? l1 : l2) ) ;
  }
  set_long( self, x, p1->len ) ;

  return (PyObject*)self ;
}


static PyMethodDef Chromosone_methods[] = {
  {"mutate", (PyCFunction)mutate, METH_VARARGS, "Randomly change bits in the chromosone. Input is chance per bit mutation"},
  {"countOnes", (PyCFunction)countOnes, METH_NOARGS, "Count number of ones in the data"},
  {"countZeros", (PyCFunction)countZeros, METH_NOARGS, "Count number of zeros in the data"},
  {"random", (PyCFunction)new_random, METH_CLASS | METH_VARARGS, "Create a new instance initialized at random of a given length"},
  {"from_parents", (PyCFunction)from_parents, METH_CLASS | METH_VARARGS, "Create a new instance inherited from 2 parents' genes"},
  {NULL, NULL, 0, NULL}
};

/********************************************************************
 *
 * Number Methods
 * 
 ********************************************************************/

static PyObject *nb_and( Chromosone *a, Chromosone *b ) {
 
  if( a->len != b->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_IndexError, "Mismatched data length for bitwise operation" ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }

  uint64_t la = as_long( a ) ;
  uint64_t lb = as_long( b ) ;
  uint64_t res = lb & la ;

  PyTypeObject *type = Py_TYPE( a ) ;
  Chromosone *self  = from_long( type, res, a->len ) ;

  return (PyObject *)self ;
}


static PyObject *nb_or( Chromosone *a, Chromosone *b ) {
 
  if( a->len != b->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_IndexError, "Mismatched data length for bitwise operation" ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }

  uint64_t la = as_long( a ) ;
  uint64_t lb = as_long( b ) ;
  uint64_t res = lb | la ;

  PyTypeObject *type = Py_TYPE( a ) ;
  Chromosone *self  = from_long( type, res, a->len ) ;

  return (PyObject *)self ;
}


static PyObject *nb_xor( Chromosone *a, Chromosone *b ) {

  if( a->len != b->len ) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyErr_SetString(PyExc_IndexError, "Mismatched data length for bitwise operation" ) ;
    PyGILState_Release(gstate);
    Py_RETURN_NONE ;
  }

  uint64_t la = as_long( a ) ;
  uint64_t lb = as_long( b ) ;
  uint64_t res = lb ^ la ;

  PyTypeObject *type = Py_TYPE( a ) ;
  Chromosone *self  = from_long( type, res, a->len ) ;

  return (PyObject *)self ;
}



static PyObject *nb_inplace_and( Chromosone *a, Chromosone *b ) {

  for( size_t i=0 ; i<a->len ; ++i ) {
    a->data[i] &= b->data[i] ;
  }
  return (PyObject *)a ;
}


static PyObject *nb_inplace_or( Chromosone *a, Chromosone *b ) {
  for( size_t i=0 ; i<a->len ; ++i ) {
    a->data[i] |= b->data[i] ;
  }
  return (PyObject *)a ;
}


static PyObject *nb_inplace_xor( Chromosone *a, Chromosone *b ) {
  for( size_t i=0 ; i<a->len ; ++i ) {
    a->data[i] ^= b->data[i] ;
  }
  return (PyObject *)a ;
}

static int nb_bool( Chromosone *a ) {
  uint64_t rc = as_long( a ) ;
  return rc != 0L ;
}

static PyObject *nb_invert( Chromosone *a ) {

  uint64_t la = as_long( a ) ;
  uint64_t res = ~la ;

  PyTypeObject *type = Py_TYPE( a ) ;
  Chromosone *self  = from_long( type, res, a->len ) ;

  return (PyObject *)self ;
}


static PyObject *nb_long( Chromosone *a ) {
  uint64_t rc = as_long( a ) ;
  return PyLong_FromLong( rc ) ;
}


static PyObject *nb_float( Chromosone *a ) {
  uint64_t val = as_long( a ) ;

  if( a->len >= sizeof(double) * 8 ) {
    return PyFloat_FromDouble( *(double*)&val ) ;
  }
  if( a->len >= sizeof(float) * 8 ) {
    return PyFloat_FromDouble( *(float*)&val ) ;
  }
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyErr_SetString(PyExc_IndexError, "Not enough bits to hold a double or float!" ) ;
  PyGILState_Release(gstate);
  Py_RETURN_NAN ;
}


static PyNumberMethods Chromosone_numbers = {
    0 ,       //  binaryfunc nb_add;
    0 ,       //  binaryfunc nb_subtract;
    0 ,       //  binaryfunc nb_multiply;
    0 ,       //  binaryfunc nb_remainder;
    0 ,       //  binaryfunc nb_divmod;
    0 ,       //  ternaryfunc nb_power;
    0 ,       //  unaryfunc nb_negative;
    0 ,       //  unaryfunc nb_positive;
    0 ,       //  unaryfunc nb_absolute;
    (inquiry)nb_bool ,          //  inquiry nb_bool;
    (unaryfunc)nb_invert ,      //  unaryfunc nb_invert;
    0 ,                         //  binaryfunc nb_lshift;
    0 ,       //  binaryfunc nb_rshift;
    (binaryfunc)nb_and ,        //  binaryfunc nb_and;
    (binaryfunc)nb_xor ,        //  binaryfunc nb_xor;
    (binaryfunc)nb_or ,         //  binaryfunc nb_or;
    (unaryfunc)nb_long ,        //  unaryfunc nb_int;
    0 ,       //  void *nb_reserved;  /* the slot formerly known as nb_long */
    (unaryfunc)nb_float ,       //  unaryfunc nb_float;

    0 ,       //  binaryfunc nb_inplace_add;
    0 ,       //  binaryfunc nb_inplace_subtract;
    0 ,       //  binaryfunc nb_inplace_multiply;
    0 ,       //  binaryfunc nb_inplace_remainder;
    0 ,       //  ternaryfunc nb_inplace_power;
    0 ,       //  binaryfunc nb_inplace_lshift;
    0 ,       //  binaryfunc nb_inplace_rshift;
    (binaryfunc)nb_inplace_and ,       //  binaryfunc nb_inplace_and;
    (binaryfunc)nb_inplace_xor ,       //  binaryfunc nb_inplace_xor;
    (binaryfunc)nb_inplace_or ,       //  binaryfunc nb_inplace_or;

    0 ,       //  binaryfunc nb_floor_divide;
    0 ,       //  binaryfunc nb_true_divide;
    0 ,       //  binaryfunc nb_inplace_floor_divide;
    0 ,       //  binaryfunc nb_inplace_true_divide;

    0 ,       //  unaryfunc nb_index;

    0 ,       //  binaryfunc nb_matrix_multiply;
    0         //  binaryfunc nb_inplace_matrix_multiply;
} ;

/********************************************************************
 *
 * Iterator Methods
 * 
 ********************************************************************/

static PyObject *Chromosone_iternext( Chromosone *iter ) {
  if ( iter->current < iter->len ) {
    return PyLong_FromLong( iter->data[iter->current++] ) ;
  }

  return NULL ;
}

/********************************************************************
 * 
 * Main Type Definition
 * 
 ********************************************************************/
static PyTypeObject ChromosoneType = {
    PyVarObject_HEAD_INIT(NULL, 0) 
    "gene_pool.Chromosone",                   /* tp_name */
    sizeof(Chromosone),                       /* tp_basicsize */
    0,                                        /* tp_itemsize */
    (destructor)Chromosone_dealloc,           /* tp_dealloc */
    0,                                        /* tp_print */
    0,                                        /* tp_getattr */
    0,                                        /* tp_setattr */
    0,                                        /* tp_reserved */
    (reprfunc)Chromosone_repr,                /* tp_repr */
    &Chromosone_numbers,                      /* tp_as_number */
    0,                                        /* tp_as_sequence */
    &Chromosone_mappings,                     /* tp_as_mapping */
    0,                                        /* tp_hash  */
    0,                                        /* tp_call */
    0,                                        /* tp_str */
    0,                                        /* tp_getattro */
    0,                                        /* tp_setattro */
    0,                                        /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    "Chromosone objects",                     /* tp_doc */
    0,                                        /* tp_traverse */
    0,                                        /* tp_clear */
    0,                                        /* tp_richcompare */
    0,                                        /* tp_weaklistoffset */
    // (getiterfunc)Chromosone_getiter,          /* tp_iter */
    PyObject_SelfIter,                        /* tp_iter */
    (iternextfunc)Chromosone_iternext,        /* tp_iternext */
    Chromosone_methods,                       /* tp_methods */
    Chromosone_members,                       /* tp_members */
    0,                                        /* tp_getset */
    0,                                        /* tp_base */
    0,                                        /* tp_dict */
    0,                                        /* tp_descr_get */
    0,                                        /* tp_descr_set */
    0,                                        /* tp_dictoffset */
    (initproc)Chromosone_init,                /* tp_init */
    0,                                        /* tp_alloc */
    Chromosone_new                            /* tp_new */
};


/*****************************************************************
 * 
 * Main Module Definition / Initialization
 * 
 *****************************************************************/
static struct PyModuleDef gene_pool_definition = {
    PyModuleDef_HEAD_INIT,
    "gene_pool",
    "example module containing Chromosone class",
    -1,
    NULL,
};

PyMODINIT_FUNC PyInit_gene_pool(void) {
  srand48( getpid() ) ;
  srand( getpid() ) ;
  Py_Initialize();
  PyObject *m = PyModule_Create(&gene_pool_definition);

  if (PyType_Ready(&ChromosoneType) < 0)
    return NULL;

  Py_INCREF(&ChromosoneType);   // class is in use

  PyModule_AddObject(m, "Chromosone", (PyObject *)&ChromosoneType);

  return m;
}

