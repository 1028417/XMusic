#pragma once

#include "Player.h"

//#define __xurl

#define __ReadStreamWaitTime 6

class __ModelExt CMediaOpaque : public CAudioOpaque
{
 public:
    CMediaOpaque() = default;

private:
	wstring m_strFile;

    UINT m_uByteRate = 0;

    const void *m_pXmscCodec = NULL;
    size_t m_uXmscHeadLen = 0;

    //uint64_t m_uPos = 0;

    uint64_t m_uWaitSize = 0;

private:
    long long _openFile(cwstr strFile, bool bXmsc);

	UINT _checkDuration()
	{
		UINT uDuration = CAudioOpaque::checkDuration();
		close();
		return uDuration;
	}

	static UINT _checkDuration(cwstr strFile, bool bXmsc, long long& nFileSize);

public:
#if __OnlineMediaLib
    bool loadXPkg(const string& strFile);

#if __xurl
    bool loadXUrl(Instream& ins);
#endif

    bool checkOpaque(cwstr strPath);
    void statOpaque();

    bool openOpaque(cwstr strPath, bool bXmsc, UINT uByteRate, unsigned long long uFileSize);

    uint64_t downloadedSize() const;

    bool waitingFlag() const
    {
        return m_uWaitSize > 0;
    }
#endif

    cwstr currentFile() const
    {
        return m_strFile;
    }

    long long openFile(cwstr strFile, bool bXmsc)
    {
        return _openFile(strFile, bXmsc);
    }

    long long openFile(IMedia& media);

	UINT checkFileDuration(cwstr strFile, long long& nFileSize)
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
		long long nFileSize = 0;
		return checkDuration(strFile, nFileSize);
		(void)nFileSize;
    }

	UINT checkMediaDuration(IMedia& media, long long& nFileSize)
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
		long long nFileSize = 0;
		return checkMediaDuration(media, nFileSize);
		(void)nFileSize;
	}

	static UINT checkDuration(cwstr strFile, long long& nFileSize)
	{
		return _checkDuration(strFile, false, nFileSize);
	}
	static UINT checkDuration(cwstr strFile)
	{
		long long nFileSize = 0;
		return checkDuration(strFile, nFileSize);
		(void)nFileSize;
	}

	static UINT checkDuration(IMedia& media, long long& nFileSize)
	{
        return _checkDuration(media.GetAbsPath(), media.isXmsc(), nFileSize);
	}
	static UINT checkDuration(IMedia& media)
	{
		long long nFileSize = 0;
		return checkDuration(media, nFileSize);
		(void)nFileSize;
	}

	void close() override;

private:
	wstring localFilePath() const override
	{
        if (m_pXmscCodec)
		{
			return L"";
		}

		return m_strFile;
	}

        int read(byte_p buf, UINT size) override;

        int64_t seek(int64_t offset, int origin) override;

#if __OnlineMediaLib
        UINT _readStream(byte_p buf, UINT size);

        bool seekable() const override;

        int64_t size() const override;
#endif
};
