#pragma once

#include "Player.h"

#define __ReadStreamWaitTime 6

class __ModelExt CMediaOpaque : public CAudioOpaque
{
public:
    CMediaOpaque() = default;

private:
    wstring m_strFile;

    const void *m_pXmscCodec = NULL;
    size_t m_uXmscHeadLen = 0;

    uint64_t m_uPos = 0;

protected:
    virtual int64_t seek(int64_t offset, int origin) override;
    virtual int read(byte_p buf, size_t size) override;

private:
    wstring localFilePath() const override
    {
        return m_strFile;
    }

    inline int64_t _open(cwstr strFile)
    {
        auto nFileSize = fsutil::GetFileSize64(strFile);
        setSize(nFileSize);
        if (nFileSize > 0)
        {
            m_strFile = strFile;
        }
        return nFileSize;
    }

    int64_t _openXmsc(cwstr strFile);

public:
#if __OnlineMediaLib
    virtual uint64_t readableSize() const
    {
        return 0;
    }

    virtual bool waitingFlag() const
    {
        return false;
    }
#endif

    virtual void close() override;

    inline int64_t open(cwstr strFile, bool bXmsc)
    {
        if (bXmsc)
        {
            return _openXmsc(strFile);
        }
        else
        {
            return _open(strFile);
        }
    }
};
