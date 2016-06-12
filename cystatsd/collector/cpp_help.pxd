from libcpp.string cimport string

cdef extern from "<ostream>" namespace "std":
    cdef cppclass ostream[T]:
        pass

cdef extern from "<sstream>" namespace "std":
    cdef cppclass ostringstream:
        ostringstream() except +
        string str()
        ostream write(char *, size_t)
        ostream write(string)
