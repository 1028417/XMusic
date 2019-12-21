#pragma once

#include "Player.h"

class __ModelExt CMediaOpaque : public CAudioOpaque
{
public:
	CMediaOpaque() {}

private:
	wstring m_strFile;

#if !__winvc
    uint64_t m_uStreamPos = 0;
#endif

	UINT m_uByteRate = 0;

    bool m_bXmsc = false;
    void *m_pXmscCodec = NULL;

private:
    long long _openFile(const wstring& strFile, bool bXmsc);

	UINT _checkDuration()
	{
		UINT uDuration = CAudioOpaque::checkDuration();
		close();
		return uDuration;
	}

	static UINT _checkDuration(const wstring& strFile, bool bXmsc, long long& nFileSize);

public:
    const wstring& currentFile() const
    {
        return m_strFile;
    }

#if !__winvc
    uint64_t streamSize() const;

    uint64_t streamPos() const
    {
        return m_uStreamPos;
    }

    void openUrl(const string& strUrl, bool bXmsc, UINT uByteRate = 0);
#endif

    long long openFile(const wstring& strFile, bool bXmsc)
    {
        return _openFile(strFile, bXmsc);
    }

    long long openFile(IMedia& media)
    {
        return _openFile(media.GetAbsPath(), media.isXmsc());
    }

	UINT checkFileDuration(const wstring& strFile, long long& nFileSize)
	{
		nFileSize = _openFile(strFile, false);
		if (nFileSize <= 0)
		{
			return 0;
		}

		return _checkDuration();
	}

	UINT checkFileDuration(const wstring& strFile)
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

	static UINT checkDuration(const wstring& strFile, long long& nFileSize)
	{
		return _checkDuration(strFile, false, nFileSize);
	}
	static UINT checkDuration(const wstring& strFile)
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
	bool isOnline() const override
	{
        return m_strFile.empty();
	}

	wstring localFilePath() const override
	{
        if (m_bXmsc)
		{
			return L"";
		}

		return m_strFile;
	}
	
#if !__winvc
    bool seekable() const override;

	int64_t seek(int64_t offset, int origin) override;

	size_t _readStream(byte_p buf, size_t bufSize);
#endif

    size_t read(byte_p buf, size_t bufSize) override;
};
