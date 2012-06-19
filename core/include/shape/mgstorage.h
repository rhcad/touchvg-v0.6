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
    //! 给定节点名称，取出一个开始节点或结束节点
    /*! 一个节点会调用两次本函数。
        \param name 节点名称
        \param index 小于0表示只有这一个子节点，否则表示可能有多个同级同名节点并指定子节点序号
        \param ended true表示本次调用时是结束节点，false表示是开始节点
        \return 是否有此节点
    */
    virtual bool readNode(const char* name, int index, bool ended) = 0;
    
    //! 给定字段名称，取出一个单字节整数
    virtual Int8  readInt8(const char* name, Int8 defvalue = 0) = 0;
    //! 给定字段名称，取出一个双字节整数
    virtual Int16 readInt16(const char* name, Int16 defvalue = 0) = 0;
    //! 给定字段名称，取出一个长整数
    virtual Int32 readInt32(const char* name, Int32 defvalue = 0) = 0;
    //! 给定字段名称，取出一个单字节整数
    virtual UInt8  readUInt8(const char* name, UInt8 defvalue = 0) = 0;
    //! 给定字段名称，取出一个双字节整数
    virtual UInt16 readUInt16(const char* name, UInt16 defvalue = 0) = 0;
    //! 给定字段名称，取出一个长整数
    virtual UInt32 readUInt32(const char* name, UInt32 defvalue = 0) = 0;
    //! 给定字段名称，取出一个布尔值
    virtual bool readBool(const char* name, bool defvalue) = 0;
    //! 给定字段名称，取出一个浮点数
    virtual float readFloat(const char* name, float defvalue = 0) = 0;
    
    //! 给定字段名称，取出浮点数数组. 传入缓冲为空时返回所需个数
    virtual UInt32 readFloatArray(const char* name, float* values, UInt32 count) = 0;
    //! 给定字段名称，取出字符串内容，不含0结束符. 传入缓冲为空时返回所需个
    virtual UInt32 readString(const char* name, wchar_t* value, UInt32 count) = 0;
    
    //! 添加一个给定节点名称的开始节点或结束节点
    /*! 一个节点会调用两次本函数。
        \param name 节点名称
        \param index 小于0表示只有这一个子节点，否则表示可能有多个同级同名节点并指定子节点序号
        \param ended true表示本次调用时是结束节点，false表示是开始节点
        \return 是否保存成功
    */
    virtual bool writeNode(const char* name, int index, bool ended) = 0;
    
    //! 添加一个给定字段名称的单字节整数
    virtual void writeInt8(const char* name, Int8 value) = 0;
    //! 添加一个给定字段名称的双字节整数
    virtual void writeInt16(const char* name, Int16 value) = 0;
    //! 添加一个给定字段名称的长整数
    virtual void writeInt32(const char* name, Int32 value) = 0;
    //! 添加一个给定字段名称的单字节整数
    virtual void writeUInt8(const char* name, UInt8 value) = 0;
    //! 添加一个给定字段名称的双字节整数
    virtual void writeUInt16(const char* name, UInt16 value) = 0;
    //! 添加一个给定字段名称的长整数
    virtual void writeUInt32(const char* name, UInt32 value) = 0;
    //! 添加一个给定字段名称的布尔值
    virtual void writeBool(const char* name, bool value) = 0;
    //! 添加一个给定字段名称的浮点数
    virtual void writeFloat(const char* name, float value) = 0;
    
    //! 添加一个给定字段名称的浮点数数组
    virtual void writeFloatArray(const char* name, const float* values, UInt32 count) = 0;
    //! 添加一个给定字段名称的字符串内容
    virtual void writeString(const char* name, const wchar_t* value) = 0;
};

#endif // __GEOMETRY_MGSTORAGE_H_
