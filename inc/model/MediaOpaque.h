#pragma once

#include "Player.h"

#define __ReadStreamWaitTime 6

class __ModelExt CMediaOpaque : public CAudioOpaque
{
 public:
    CMediaOpaque() = default;

protected:
    UINT m_uByteRate = 0;

private:
    const void *m_pXmscCodec = NULL;
    size_t m_uXmscHeadLen = 0;

    wstring m_strFile;

protected:
    virtual int64_t seek(int64_t offset, int origin) override;
    virtual int read(byte_p buf, UINT size) override;

    void _decode(byte_p buf, int size);

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

    inline UINT checkDuration()
    {
        UINT uDuration = CAudioOpaque::checkDuration();
        close();
        return uDuration;
    }

    static UINT _checkDuration(cwstr strFile, bool bXmsc, int64_t& nFileSize);

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

    UINT checkFileDuration(cwstr strFile, int64_t& nFileSize)
    {
        nFileSize = _open(strFile);
        if (nFileSize <= 0)
        {
            return 0;
        }
        return checkDuration();
    }
    UINT checkFileDuration(cwstr strFile)
    {
        int64_t nFileSize = 0;
        return checkDuration(strFile, nFileSize);
        (void)nFileSize;
    }

    UINT checkMediaDuration(IMedia& media, int64_t& nFileSize)
    {
        nFileSize = open(media.GetAbsPath(), media.isXmsc());
        if (nFileSize <= 0)
        {
            return 0;
        }
        return checkDuration();
    }
    UINT checkMediaDuration(IMedia& media)
    {
        int64_t nFileSize = 0;
        return checkMediaDuration(media, nFileSize);
        (void)nFileSize;
    }

    static UINT checkDuration(cwstr strFile, int64_t& nFileSize)
    {
        return _checkDuration(strFile, false, nFileSize);
    }
    static UINT checkDuration(cwstr strFile)
    {
        int64_t nFileSize = 0;
        return checkDuration(strFile, nFileSize);
        (void)nFileSize;
    }

    static UINT checkDuration(IMedia& media, int64_t& nFileSize)
    {
        return _checkDuration(media.GetAbsPath(), media.isXmsc(), nFileSize);
    }
    static UINT checkDuration(IMedia& media)
    {
        int64_t nFileSize = 0;
        return checkDuration(media, nFileSize);
        (void)nFileSize;
    }
};
