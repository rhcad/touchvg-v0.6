//! \file mgstorage.h
//! \brief 定义图形存取接口 MgStorage
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSTORAGE_H_
#define __GEOMETRY_MGSTORAGE_H_

#include <mgtype.h>

//! 图形存取接口
/*! \ingroup GEOM_SHAPE
    \interface MgStorage
*/
struct MgStorage
{
    virtual bool readNode(const char* type) = 0;
    virtual Int8  getInt8(const char* name, Int8 defvalue = 0) = 0;
    virtual Int16 getInt16(const char* name, Int16 defvalue = 0) = 0;
    virtual Int32 getInt32(const char* name, Int32 defvalue = 0) = 0;
    virtual UInt8  getUInt8(const char* name, UInt8 defvalue = 0) = 0;
    virtual UInt16 getUInt16(const char* name, UInt16 defvalue = 0) = 0;
    virtual UInt32 getUInt32(const char* name, UInt32 defvalue = 0) = 0;
    virtual bool getBool(const char* name, bool defvalue) = 0;
    virtual float getFloat(const char* name, float defvalue = 0) = 0;
    virtual UInt32 getString(const char* name, wchar_t* value, UInt32 count) = 0;
    
    virtual bool writeNode(const char* type) = 0;
    virtual void setInt8(const char* name, Int8 value) = 0;
    virtual void setInt16(const char* name, Int16 value) = 0;
    virtual void setInt32(const char* name, Int32 value) = 0;
    virtual void setUInt8(const char* name, UInt8 value) = 0;
    virtual void setUInt16(const char* name, UInt16 value) = 0;
    virtual void setUInt32(const char* name, UInt32 value) = 0;
    virtual void setBool(const char* name, bool value) = 0;
    virtual void setFloat(const char* name, float value) = 0;
    virtual void setString(const char* name, const wchar_t* value) = 0;
};

#endif // __GEOMETRY_MGSTORAGE_H_
