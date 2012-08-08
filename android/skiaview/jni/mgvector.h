/* File : mgvector.h */
// Some template definitions
#ifndef SWIG_MGVECTOR_H
#define SWIG_MGVECTOR_H

template<class T> class mgvector {
    T *v;
    int sz;
public:
    mgvector(int _sz) {
        v = new T[_sz];
        sz = _sz;
    }
    mgvector(const T *_v, int _sz) {
        v = new T[_sz];
        sz = _sz;
        for (int i = 0; i < sz; i++)
            v[i] = _v[i];
    }
    T get(int index) {
        return v[index];
    }
    void set(int index, T val) {
        v[index] = val;
    }
#ifdef SWIG
    %extend {
        T getitem(int index) {
            return $self->get(index);
        }
        void setitem(int index, T val) {
            $self->set(index,val);
        }
    }
#endif
};

#endif // SWIG_MGVECTOR_H
