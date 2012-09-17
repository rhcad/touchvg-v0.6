//! \file mgstoragebs.h
//! \brief 定义序列化基类 MgStorageBase
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSTORAGEBASE_H_
#define __GEOMETRY_MGSTORAGEBASE_H_

#ifndef SWIG
#include "mgstorage.h"
#include <wchar.h>
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
    
    virtual int readInt(const char* name, int defvalue) { return name && defvalue; }
    virtual bool readBool(const char* name, bool defvalue) { return name && defvalue; }
    virtual float readFloat(const char* name, float defvalue) { return name && defvalue; }
    
    virtual bool writeNode(const char* name, int index, bool ended) {
        return name && index && ended; }
    
    virtual void writeInt(const char* name, int value) { if (name && value) value=0; }
    virtual void writeBool(const char* name, bool value) { if (name && value) value=false; }
    virtual void writeFloat(const char* name, float value) { if (name && value) value=0; }
    
    virtual int readFloatArray(const char* name, mgvector<float>& values) {
        return name && values.count(); }
    virtual void writeFloatArray(const char* name, const mgvector<float>& values) {
        name=values.count() ? name:NULL; }
    
    virtual int readString(const char* name, mgvector<short>& value) {
        return name && value.count(); }
    virtual void writeString(const char* name, const mgvector<short>& value) {
        if (name && value.count()) name=NULL; }

private:
    virtual int readFloatArray(const char* name, float* values, int count) {
        mgvector<float> arr(values, count);
        int n = readFloatArray(name, arr);
        for (int i = (n < count ? n : count) - 1; i >= 0; i--)
            values[i] = arr.get(i);
        return n;
    }
    virtual int readString(const char* name, wchar_t* value, int count) {
        mgvector<short> arr(value, count);
        int n = readString(name, arr);
        for (int i = (n < count ? n : count) - 1; i >= 0; i--)
            value[i] = (wchar_t)arr.get(i);
        return n;
    }
    virtual void writeFloatArray(const char* name, const float* values, int count) {
        mgvector<float> arr(values, count);
        writeFloatArray(name, arr);
    }
    virtual void writeString(const char* name, const wchar_t* value) {
        mgvector<short> arr(value, wcslen(value));
        writeString(name, arr);
    }
};

#endif // __GEOMETRY_MGSTORAGEBASE_H_
