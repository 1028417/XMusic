#pragma once

#include "Player.h"

#define __ReadStreamWaitTime 6

struct tagXUrl
{
    tagXUrl() = default;

    tagXUrl(const string& strUtf8Title, const string& strUrl, cwstr strFileTitle)
        : m_strUtf8Title(strUtf8Title)
        , m_strUrl(strUrl)
        , m_strFileTitle(strFileTitle)
    {
    }

    string m_strUtf8Title;
    string m_strUrl;
    wstring m_strFileTitle;
    bool bUsing = false;

    string genUrl();
};

class __ModelExt CMediaOpaque : public CAudioOpaque
{
 public:
    CMediaOpaque() = default;

#if __OnlineMediaLib
private:
    unordered_map<wstring, const tagUnzfile*> m_mapUnzfile;

    unordered_map<wstring, tagXUrl> m_mapXUrl;

private:
    void _addXUrl(const string& strUtf8Title, const string& strUrl);

public:
    bool loadPkg(const string& strFile);

    bool checkMedia(cwstr strPath);

    string getMedia(cwstr strPath);

    void checkUnuse();

    bool loadXUrl(Instream& ins);
#endif

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
    cwstr currentFile() const
    {
        return m_strFile;
    }

#if __OnlineMediaLib
    uint64_t downloadedSize() const;

    bool waitingFlag() const
    {
        return m_uWaitSize > 0;
    }
	
    void openUrl(const string& strUrl, bool bXmsc, UINT uByteRate, unsigned long long uFileSize);
#endif

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
