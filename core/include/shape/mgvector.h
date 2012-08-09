// mgvector.h: template vector definition
#ifndef SWIG_MGVECTOR_H
#define SWIG_MGVECTOR_H

template<class T> class mgvector {
    T *v;
    int sz;
public:
    mgvector(int _sz) {
        v = _sz > 0 ? new T[_sz] : NULL;
        sz = _sz;
    }
    mgvector(const T *_v, int _sz) {
        v = _sz > 0 ? new T[_sz] : NULL;
        sz = _sz;
        for (int i = 0; i < sz; i++)
            v[i] = _v[i];
    }
    ~mgvector() {
        delete[] v;
    }
    int count() const {
        return sz;
    }
    T get(int index) const {
        return v[index];
    }
    void set(int index, T val) {
        v[index] = val;
    }
};

#endif // SWIG_MGVECTOR_H
