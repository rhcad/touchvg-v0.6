//! \file mgstoragebs.h
//! \brief 定义序列化基类 MgStorageBase
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSTORAGEBASE_H_
#define __GEOMETRY_MGSTORAGEBASE_H_

#include "mgstorage.h"
#include "mgvector.h"

//! 序列化基类
/*! \ingroup GEOM_SHAPE
 */
class MgStorageBase : public MgStorage
{
public:
    MgStorageBase() {}
    virtual ~MgStorageBase() {}
    
    virtual int readFloatArray(const char* name, mgvector<float>& values) = 0;
    virtual void writeFloatArray(const char* name, const mgvector<float>& values) = 0;
    virtual int readString(const char* name, mgvector<char>& value) = 0;

private:
    virtual int readFloatArray(const char* name, float* values, int count) {
        mgvector<float> arr(values, count);
        int n = readFloatArray(name, arr);
        if (values && count > 0) {
            n = n < count ? n : count;
            for (int i = 0; i < n; i++)
                values[i] = arr.get(i);
        }
        return n;
    }
    virtual int readString(const char* name, char* value, int count) {
        mgvector<char> arr(value, count);
        int n = readString(name, arr);
        if (value && count > 0) {
            n = n < count ? n : count;
            for (int i = 0; i < n; i++)
                value[i] = arr.get(i);
        }
        return n;
    }
    virtual void writeFloatArray(const char* name, const float* values, int count) {
        mgvector<float> arr(values, count);
        writeFloatArray(name, arr);
    }
};

#endif // __GEOMETRY_MGSTORAGEBASE_H_
