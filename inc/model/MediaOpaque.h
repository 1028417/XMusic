#pragma once

#include "Player.h"

//#define __xurl

#define __ReadStreamWaitTime 6

class __ModelExt CMediaOpaque : public CAudioOpaque
{
 public:
    CMediaOpaque() = default;

private:
    UINT m_uByteRate = 0;

#if __OnlineMediaLib
    bool m_bOnlineFile = false;
    uint64_t m_uWaitSize = 0;
#endif

    CZipFile *m_pXPkg = NULL;

    const void *m_pXmscCodec = NULL;
    size_t m_uXmscHeadLen = 0;

    wstring m_strFile;

    //uint64_t m_uPos = 0;

private:
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
    bool loadXPkg(const string& strFile);

#if __xurl
    bool loadXUrl(Instream& ins);
#endif

    bool checkOpaque(cwstr strPath);
    void statOpaque();

    bool openOpaque(IMedia& media);

    uint64_t downloadedSize() const;

    bool waitingFlag() const
    {
        return m_uWaitSize > 0;
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

	void close() override;

private:
	wstring localFilePath() const override
	{
        if (!m_bOnlineFile && NULL == m_pXPkg && NULL == m_pXmscCodec)
		{
            return m_strFile;
		}

        return L"";
	}

        int read(byte_p buf, UINT size) override;

        int64_t seek(int64_t offset, int origin) override;

#if __OnlineMediaLib
        UINT _readStream(byte_p buf, UINT size);

        bool seekable() const override;

        int64_t size() const override;
#endif
};
