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

    //uint64_t m_uPos = 0;

private:
    wstring localFilePath() const override
    {
        return m_strFile;
    }

    int64_t _openFile(cwstr strFile, bool bXmsc);

	UINT _checkDuration()
	{
		UINT uDuration = CAudioOpaque::checkDuration();
		close();
		return uDuration;
	}

	static UINT _checkDuration(cwstr strFile, bool bXmsc, int64_t& nFileSize);

public:
#if __OnlineMediaLib
    virtual bool waitingFlag() const
    {
        return false;
    }
#endif

    int64_t openFile(cwstr strFile, bool bXmsc)
    {
        return _openFile(strFile, bXmsc);
    }

    int64_t openFile(IMedia& media);

    UINT checkFileDuration(cwstr strFile, int64_t& nFileSize)
    {
        nFileSize = _openFile(strFile, false);
        if (nFileSize <= 0)
        {
            return 0;
        }

        return _checkDuration();
    }

    UINT checkFileDuration(cwstr strFile)
    {
        int64_t nFileSize = 0;
        return checkDuration(strFile, nFileSize);
        (void)nFileSize;
    }

    UINT checkMediaDuration(IMedia& media, int64_t& nFileSize)
    {
        nFileSize = _openFile(media.GetAbsPath(), media.isXmsc());
        if (nFileSize <= 0)
        {
            return 0;
        }

        return _checkDuration();
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

    virtual int read(byte_p buf, UINT size) override;

    virtual int64_t seek(int64_t offset, int origin) override;

    virtual void close() override;

protected:
    void _decode(byte_p buf, int size);
};
