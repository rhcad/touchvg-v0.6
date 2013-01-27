//! \file mgjsonstorage.h
//! \brief 定义JSON序列化类 MgJsonStorage
// License: LGPL, https://github.com/rhcad/touchvg
#ifndef __GEOMETRY_CORE_JSONSTORAGE_H_
#define __GEOMETRY_CORE_JSONSTORAGE_H_

struct MgStorage;

//! JSON序列化类
/*! \ingroup GEOM_SHAPE
 */
class MgJsonStorage
{
public:
    MgJsonStorage();
    ~MgJsonStorage();

    //! 给定JSON内容，返回存取接口对象以便开始读取
    MgStorage* storageForRead(const char* content);

    //! 返回存取接口对象以便开始写数据，写完可调用 stringify()
    MgStorage* storageForWrite();

    //! 返回JSON内容
    const char* stringify(bool pretty);

    //! 返回 storageForRead() 中的解析错误，NULL表示没有错误
    const char* getParseError();

private:
    class Impl;
    Impl* _impl;
};

#endif // __GEOMETRY_CORE_JSONSTORAGE_H_
