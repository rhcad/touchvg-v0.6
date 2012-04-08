//! \file mgstorage.h
//! \brief 定义图形存取接口 MgStorage
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGSTORAGE_H_
#define __GEOMETRY_MGSTORAGE_H_

#include <mgbox.h>

//! 图形存取接口
/*! \ingroup GEOM_SHAPE
    \interface MgStorage
*/
struct MgStorage
{
    virtual Int8  getInt8(const wchar_t* name, Int8 defvalue = 0) = 0;
    virtual Int16 getInt16(const wchar_t* name, Int16 defvalue = 0) = 0;
    virtual Int32 getInt32(const wchar_t* name, Int32 defvalue = 0) = 0;
    virtual UInt8  getUInt8(const wchar_t* name, UInt8 defvalue = 0) = 0;
    virtual UInt16 getUInt16(const wchar_t* name, UInt16 defvalue = 0) = 0;
    virtual UInt32 getUInt32(const wchar_t* name, UInt32 defvalue = 0) = 0;
    virtual bool getBool(const wchar_t* name, bool defvalue) = 0;
    virtual float getFloat(const wchar_t* name, float defvalue = 0) = 0;
    virtual double getDouble(const wchar_t* name, double defvalue = 0) = 0;
    virtual UInt32 getString(const wchar_t* name, wchar_t* value, UInt32 count) = 0;
    
    virtual void setInt8(const wchar_t* name, Int8 value) = 0;
    virtual void setInt16(const wchar_t* name, Int16 value) = 0;
    virtual void setInt32(const wchar_t* name, Int32 value) = 0;
    virtual void setUInt8(const wchar_t* name, UInt8 value) = 0;
    virtual void setUInt16(const wchar_t* name, UInt16 value) = 0;
    virtual void setUInt32(const wchar_t* name, UInt32 value) = 0;
    virtual void setBool(const wchar_t* name, bool value) = 0;
    virtual void setFloat(const wchar_t* name, float value) = 0;
    virtual void setDouble(const wchar_t* name, double value) = 0;
    virtual void setString(const wchar_t* name, const wchar_t* value) = 0;
    
    virtual void getPoint(const wchar_t* name, Point2d& value);
    virtual void getVector(const wchar_t* name, Vector2d& value);
    virtual void getMatrix(const wchar_t* name, Matrix2d& value);
    virtual void setPoint(const wchar_t* name, const Point2d& value);
    virtual void setVector(const wchar_t* name, const Vector2d& value);
    virtual void setMatrix(const wchar_t* name, const Matrix2d& value);
    
    virtual UInt32 getPoints(const wchar_t* name, Point2d* value, UInt32 count) = 0;
    virtual void setPoints(const wchar_t* name, const Point2d* value, UInt32 count) = 0;
};

#endif // __GEOMETRY_MGSTORAGE_H_
