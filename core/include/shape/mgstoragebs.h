//! \file mgstoragebs.h
//! \brief 定义序列化基类 MgStorageBase
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSTORAGEBASE_H_
#define __GEOMETRY_MGSTORAGEBASE_H_

#ifndef SWIG
#include "mgstorage.h"
#endif
#include "mgvector.h"

//! 序列化基类
/*! \ingroup GEOM_SHAPE
 */
class MgStorageBase : public MgStorage
{
public:
    MgStorageBase() {}
    virtual ~MgStorageBase() {}
    
    virtual bool readNode(const char* name, int index, bool ended) {
        return name && index && ended; }
    
    virtual int readInt(const char* name, int defvalue = 0) { return name && defvalue; }
    virtual bool readBool(const char* name, bool defvalue) { return name && defvalue; }
    virtual float readFloat(const char* name, float defvalue = 0) { return name && defvalue; }
    
    virtual int readFloatArray(const char* name, mgvector<float>& values) {
        return name && values.count(); }
    virtual int readString(const char* name, mgvector<char>& value) {
        return name && value.count(); }
    
    virtual bool writeNode(const char* name, int index, bool ended) {
        return name && index && ended; }
    
    virtual void writeInt(const char* name, int value) { if (name) value=0; }
    virtual void writeBool(const char* name, bool value) { if (name) value=false; }
    virtual void writeFloat(const char* name, float value) { if (name) value=0; }
    
    virtual void writeFloatArray(const char* name, const mgvector<float>& values) {
        name=values.count() ? name:NULL; }
    virtual void writeString(const char* name, const char* value) { if (name) value=NULL; }

private:
    virtual int readFloatArray(const char* name, float* values, int count) {
        mgvector<float> arr(values, count);
        int n = readFloatArray(name, arr);
        for (int i = 0; i < n; i++)
            values[i] = arr.get(i);
        return n;
    }
    virtual int readString(const char* name, char* value, int count) {
        mgvector<char> arr(value, count);
        int n = readString(name, arr);
        for (int i = 0; i < n; i++)
            value[i] = arr.get(i);
        return n;
    }
    virtual void writeFloatArray(const char* name, const float* values, int count) {
        mgvector<float> arr(values, count);
        writeFloatArray(name, arr);
    }
};

#endif // __GEOMETRY_MGSTORAGEBASE_H_
