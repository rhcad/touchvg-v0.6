#include "stdafx.h"
#include "CmdLineParams.h"

static CStringArray s_tokens;
static CStringArray s_pics;
static CStringArray s_lrcs;

static bool GetToken(CString& token, LPCTSTR& cmdLine)
{
    token.Empty();
	while (iswspace(*cmdLine))      // 跳过空白字符
		cmdLine++;
	
	LPCTSTR start = cmdLine;        // 第一个非空白字符

	if ('\"' == *start)            // 如果以引号开始
	{
		while (*(++cmdLine) && *cmdLine != '\"') ;         // 找到下一个引号
		token = CString(start + 1, cmdLine - start - 1);    // 取两个引号之间的
	}
	else
	{
        while (*cmdLine && !iswspace(*(++cmdLine)))         // 找下一个空白字符
        {
            if ('\"' == *cmdLine)                          // 遇到引号
            {
                token = CString(start, cmdLine - start);    // 先取引号前的
                start = cmdLine;
                while (*(++cmdLine) && *cmdLine != '\"') ; // 找到下一个引号，不检查空格
                token += CString(start + 1, cmdLine - start - 1);    // 加上后面的
                break;                                      // 不再找空白字符
            }
        }
        if (token.IsEmpty())
		    token = CString(start, cmdLine - start);
	}

	if (*cmdLine)
		cmdLine++;                                  // 跳过空白字符或引号

	return !token.IsEmpty();
}

static bool _FindCmdLineParam(LPCTSTR name, LPCTSTR* value, bool remove)
{
    int i = s_tokens.GetSize();
    LPCTSTR post = _T("");
    static CString token;

    while (--i >= 0)
    {
        token = s_tokens.GetAt(i);

        if (!token.IsEmpty() && token[0] == '-' && token.Find(name, 1) >= 0)
        {
            post = (LPCTSTR)token + 1 + _tcslen(name);
            if (*post == 0 || *post == ':' || *post == '=')
            {
                if (*post != 0)
                    post++;
                if (*post == 0 && i + 1 < s_tokens.GetSize())
                {
                    token = s_tokens.GetAt(i + 1);
                    post = (LPCTSTR)token;
                    if (*post == '-')
                        post = _T("");
                    if (remove)
                        s_tokens.RemoveAt(i + 1);
                }
                if (remove)
                    s_tokens.RemoveAt(i);
                break;
            }
        }
    }
    if (value) {
        *value = post;
    }

    return i >= 0;
}

bool SetCmdLineParams(LPCTSTR cmdLine)
{
    CString token;
    LPCTSTR value;

    // 记下上一命令行中的导入文件名
    while (FindCmdLineParam(_T("pic"), &value))
    {
        s_pics.Add(value);
        _FindCmdLineParam(_T("pic"), NULL, true);
        
        FindCmdLineParam(_T("lrc"), &value);
        s_lrcs.Add(value);
        _FindCmdLineParam(_T("lrc"), NULL, true);
    }

    // 改变命令行
    s_tokens.RemoveAll();
	while (GetToken(token, cmdLine))
	{
		if (token[0] == '/')
			token.SetAt(0, '-');
        s_tokens.Add(token);
	}

    return s_tokens.GetSize() > 0;
}

bool FindCmdLineParam(LPCTSTR name, LPCTSTR* value)
{
    return _FindCmdLineParam(name, value, false);
}

bool RemoveCmdLineParam(LPCTSTR name)
{
    bool ret = _FindCmdLineParam(name, NULL, true);

    if (_tcscmp(name, _T("pic")) == 0 && s_pics.GetSize() > 0)
    {
        s_tokens.Add(_T("-pic:"));
        s_tokens.Add(s_pics[0]);
        s_pics.RemoveAt(0);
    }

    if (_tcscmp(name, _T("lrc")) == 0 && s_lrcs.GetSize() > 0)
    {
        s_tokens.Add(_T("-lrc:"));
        s_tokens.Add(s_lrcs[0]);
        s_lrcs.RemoveAt(0);
    }

    return ret;
}
