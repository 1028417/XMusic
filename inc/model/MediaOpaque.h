#pragma once

#include "Player.h"

#include "FileDownload.h"

class __ModelExt CMediaOpaque : public CAudioOpaque
{
public:
	CMediaOpaque() {}

    ~CMediaOpaque();

private:
	wstring m_strFile;

    bool m_bUrl = false;

	void *m_pXmsc = NULL;

private:
	int _openFile(const wstring& strFile, bool bXmsc);

	UINT _checkDuration()
	{
		UINT uDuration = CAudioOpaque::checkDuration();
		close();
		return uDuration;
	}

	static UINT _checkDuration(const wstring& strFile, bool bXmsc, int& nFileSize);

public:
    int openFile(const wstring& strFile)
	{
		return _openFile(strFile, false);
    }

	int openFile(IMedia& media)
	{
		return _openFile(media.GetAbsPath(), media.isXmsc());
	}
	
	void openUrl(const string& strUrl);

#if __winvc
	UINT checkFileDuration(const wstring& strFile, int& nFileSize)
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
		int nFileSize = 0;
		return checkDuration(strFile, nFileSize);
		(void)nFileSize;
	}
#endif

	UINT checkMediaDuration(IMedia& media, int& nFileSize)
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
		int nFileSize = 0;
		return checkMediaDuration(media, nFileSize);
		(void)nFileSize;
	}

	static UINT checkDuration(const wstring& strFile, int& nFileSize)
	{
		return _checkDuration(strFile, false, nFileSize);
	}
	static UINT checkDuration(const wstring& strFile)
	{
		int nFileSize = 0;
		return checkDuration(strFile, nFileSize);
		(void)nFileSize;
	}

	static UINT checkDuration(IMedia& media, int& nFileSize)
	{
        return _checkDuration(media.GetAbsPath(), media.isXmsc(), nFileSize);
	}
	static UINT checkDuration(IMedia& media)
	{
		int nFileSize = 0;
		return checkDuration(media, nFileSize);
		(void)nFileSize;
	}

public:
	wstring file() const override 
	{
		return m_strFile;
	}

	void close() override;

private:
    int read(uint8_t *buf, size_t size) override;
};
