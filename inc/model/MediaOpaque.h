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
    long long openFile(const wstring& strFile, bool bXmsc)
	{
		return _openFile(strFile, bXmsc);
    }

	long long openFile(IMedia& media)
	{
		return _openFile(media.GetAbsPath(), media.isXmsc());
	}

#if !__winvc
    void openUrl(const string& strUrl, bool bXmsc, UINT uByteRate = 0);

    uint64_t streamSize() const;

    uint64_t streamPos() const
    {
        return m_uStreamPos;
    }

    void cutData(uint64_t uPos) const;

#else
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
#endif

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

public:
	void close() override;

	const wstring& currentFile() const
	{
		return m_strFile;
	}

    int readHead(byte_t *buf, size_t size);

private:
#if !__winvc
    long checkPreserveDataSize() const override;
#endif

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

    UINT byteRate() const override
    {
#if !__winvc
		if (m_strFile.empty())
		{
			return m_uByteRate;
		}
#endif

		return CAudioOpaque::byteRate();	
    }

    int read(byte_t *buf, size_t size) override;
};
