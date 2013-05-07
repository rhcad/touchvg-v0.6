//! \file MgStorageJson.java
//! \brief 定义JSON序列化实现类 MgStorageJson
// License: LGPL, https://github.com/rhcad/touchvg

package touchvg.view;

import org.apache.http.util.EncodingUtils;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import touchvg.jni.MgStorage;
import touchvg.jni.MgJsonStorage;

//! JSON序列化实现类
/*! \ingroup GRAPH_SKIA
    \see MgJsonStorage, MgStorage
 */
public class MgStorageJson extends Object {

    private MgJsonStorage mStorage = new MgJsonStorage();
    private String content = null;

    /// 兼容以前版本
    public void delete()
    {
        mStorage.delete();
    }

    /// 返回存取接口对象以便开始写数据，写完可调用 stringify()
    public MgStorage storageForWrite()
    {
        return mStorage.storageForWrite();
    }

    /// 返回 storageForWrite 写完后的JSON内容
    public String getContent()
    {
        return mStorage.stringify(true);
    }

    /// storageForWrite 写完后保存JSON内容到指定的文件
    public boolean writeFile(String filename)
    {
        String content = getContent();
        int savelen = 0;

        if (content != null && content.length() > 0) {
            File file = new File(filename);
            File pf = file.getParentFile();
            if (!pf.exists()) {
                pf.mkdirs();
            }
            if (!file.exists()) {
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                    return false;
                }
            }

            try {
                FileOutputStream fout = new FileOutputStream(file);
                byte [] bytes = content.getBytes();
                fout.write(bytes);
                fout.close();
                savelen = bytes.length;
                bytes = null;
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }

        return savelen > 0;
    }

    /// 指定JSON内容，后续调用 storageForRead
    public void setContent(String content) {
        this.content = content;
    }

    /// 从文件中读取内容，后续调用 storageForRead
    public boolean readFile(String filename)
    {
        File file = new File(filename);

        this.content = null;
        if (!file.exists() || !file.isFile()) {
            return false;
        }
        try {
            FileInputStream fin = new FileInputStream(file);
            int length = fin.available();
            if (length > 0) {
                byte [] buffer = new byte[length];
                fin.read(buffer);
                this.content = EncodingUtils.getString(buffer, "UTF-8");
            }
            fin.close();
            return length > 0;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    /// 调用 setContent 或 readFile 后调用本函数，返回存取接口对象以便开始读取
    public MgStorage storageForRead()
    {
        return mStorage.storageForRead(this.content);
    }
}
