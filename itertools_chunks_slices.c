#include <Python.h>

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    Py_ssize_t cnt;
} chunksobject;

static PyTypeObject chunks_type;

static PyObject *
chunks_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "n", NULL};
    chunksobject *ro;
    PyObject *seq;
    PyObject *iterator;
    Py_ssize_t cnt = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "On:chunks", kwargs,
                                     &seq, &cnt))
        return NULL;

    if (cnt <= 0) {
        PyErr_SetString(PyExc_ValueError, "n must be positive");
        return NULL;
    }

    iterator = PyObject_GetIter(seq);
    if (iterator == NULL)
        return NULL;

    ro = (chunksobject *)type->tp_alloc(type, 0);
    if (ro == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }

    ro->iterator = iterator;
    ro->cnt = cnt;
    return (PyObject *)ro;
}

static void
chunks_dealloc(chunksobject *ro)
{
    PyObject_GC_UnTrack(ro);
    Py_XDECREF(ro->iterator);
    Py_TYPE(ro)->tp_free(ro);
}

static int
chunks_traverse(chunksobject *ro, visitproc visit, void *arg)
{
    Py_VISIT(ro->iterator);
    return 0;
}

static PyObject *
chunks_next(chunksobject *ro)
{
    Py_ssize_t cnt = ro->cnt;
    PyObject *it = ro->iterator;
    PyObject *(*iternext)(PyObject *);

    iternext = *Py_TYPE(it)->tp_iternext;

    PyObject *tuple;
    tuple = PyTuple_New(cnt);
    if (tuple == NULL)
        return NULL;

    for (int i = 0; i < cnt; ++i) {
        PyObject *elem = iternext(it);
        if (elem == NULL) {
            Py_DECREF(tuple);
            return NULL;
        }
        PyTuple_SET_ITEM(tuple, i, elem);
    }

    return tuple;
}

static PyObject *
chunks_repr(chunksobject *ro)
{
    return PyUnicode_FromFormat("chunks(%R, %zd)", ro->iterator, ro->cnt);
}

PyDoc_STRVAR(chunks_doc,
"chunks(object, times) -> ");

static PyTypeObject chunks_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "itertools_chunks_slices.chunks",   /* tp_name */
    sizeof(chunksobject),               /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)chunks_dealloc,         /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    (reprfunc)chunks_repr,              /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    chunks_doc,                         /* tp_doc */
    (traverseproc)chunks_traverse,      /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)chunks_next,          /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    chunks_new,                         /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};

// *********************************************************

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *previous; // tuple
    Py_ssize_t cnt;
} slicesobject;

static PyTypeObject slices_type;

static PyObject *
slices_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "n", NULL};
    slicesobject *ro;
    PyObject *seq;
    PyObject *iterator;
    Py_ssize_t cnt = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "On:slices", kwargs,
                                     &seq, &cnt))
        return NULL;

    if (cnt <= 0) {
        PyErr_SetString(PyExc_ValueError, "n must be positive");
        return NULL;
    }

    iterator = PyObject_GetIter(seq);
    if (iterator == NULL)
        return NULL;

    ro = (slicesobject *)type->tp_alloc(type, 0);
    if (ro == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }

    ro->iterator = iterator;
    ro->previous = NULL;
    ro->cnt = cnt;
    return (PyObject *)ro;
}

static void
slices_dealloc(slicesobject *ro)
{
    PyObject_GC_UnTrack(ro);
    Py_XDECREF(ro->iterator);
    Py_XDECREF(ro->previous);
    Py_TYPE(ro)->tp_free(ro);
}

static int
slices_traverse(slicesobject *ro, visitproc visit, void *arg)
{
    Py_VISIT(ro->iterator);
    Py_VISIT(ro->previous);
    return 0;
}

static PyObject *
slices_next(slicesobject *ro)
{
    Py_ssize_t cnt = ro->cnt;
    PyObject *it = ro->iterator;
    PyObject *previous = ro->previous;
    PyObject *(*iternext)(PyObject *);

    iternext = *Py_TYPE(it)->tp_iternext;

    PyObject *tuple;
    tuple = PyTuple_New(cnt);
    if (tuple == NULL)
        return NULL;
    if (previous == NULL) {
        for (int i = 0; i < cnt; ++i) {
            PyObject *elem = iternext(it);
            if (elem == NULL) {
                Py_DECREF(tuple);
                return NULL;
            }
            PyTuple_SET_ITEM(tuple, i, elem);
        }
    } else {
        PyObject *elem = iternext(it);
        if (elem == NULL) {
            Py_DECREF(tuple);
            return NULL;
        }
        for (int i = 0; i < cnt-1; ++i) {
            PyObject *temp = PyTuple_GET_ITEM(previous, i+1);
            Py_INCREF(temp);
            PyTuple_SET_ITEM(tuple, i, temp);
        }

        PyTuple_SET_ITEM(tuple, cnt-1, elem);

        Py_DECREF(previous);
    }
    Py_INCREF(tuple);
    ro->previous = tuple;
    return tuple;
}

static PyObject *
slices_repr(slicesobject *ro)
{
    return PyUnicode_FromFormat("slices(%R, %zd)", ro->iterator, ro->cnt);
}

PyDoc_STRVAR(slices_doc,
"slices(object, times) -> ");

static PyTypeObject slices_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "itertools_chunks_slices.slices",   /* tp_name */
    sizeof(slicesobject),               /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)slices_dealloc,         /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    (reprfunc)slices_repr,              /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    slices_doc,                         /* tp_doc */
    (traverseproc)slices_traverse,      /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)slices_next,          /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    slices_new,                         /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};

static struct PyModuleDef extensionsmodule = {
   PyModuleDef_HEAD_INIT, "itertools_chunks_slices", NULL, -1, NULL
};

PyMODINIT_FUNC
PyInit_itertools_chunks_slices(void)
{
	PyObject *m;
    m = PyModule_Create(&extensionsmodule);
    if (m == NULL)
    	return NULL;

    PyTypeObject *typelist[] = {
        &chunks_type,
        &slices_type,
        NULL
    };

    for (int i = 0 ; typelist[i] != NULL ; i++) {
        if (PyType_Ready(typelist[i]) < 0)
            return NULL;
        char *name = strchr(typelist[i]->tp_name, '.');
        assert (name != NULL);
        Py_INCREF(typelist[i]);
        PyModule_AddObject(m, name+1, (PyObject *)typelist[i]);
    }
    return m;
}