#pragma once

#include "Player.h"

#include "FileDownload.h"

class __ModelExt CMediaOpaque : public CAudioOpaque
{
public:
    CMediaOpaque() {}

	CMediaOpaque(const wstring& strFile, bool bXmsc = false)
	{
		setFile(strFile, bXmsc);
	}

private:
    string m_strUrl;
    CFileDownload m_fileDownload;

    void *m_pXmsc = NULL;

public:
	/*const string& url() const
	{
		return m_strUrl;
	}*/

	const wstring& file() const
	{
		return CAudioOpaque::m_strFile;
	}

        int setFile(const wstring& strFile, bool bXmsc = false);
	void setUrl(const string& strUrl);

	int checkDuration()
	{
		return CAudioOpaque::checkDuration();
	}

	static int checkDuration(const wstring& strFile)
	{
		return _checkDuration(strFile, false); // TODO
	}

	static int checkDuration(IMedia& media)
	{
		bool bXmsc = media.GetFileType() == E_MediaFileType::MFT_Null;
		return _checkDuration(media.GetAbsPath(), bXmsc);
	}

private:
	static int _checkDuration(const wstring& strFile, bool bXmsc)
	{
		static mutex s_mutex;
		mutex_lock lock(s_mutex);

		static CMediaOpaque MediaOpaque;
		MediaOpaque.setFile(strFile, bXmsc);
		return MediaOpaque.checkDuration();
	}

	wstring getFile() const override
	{
		if (m_pXmsc)
		{
			return L"";
		}

		return CAudioOpaque::getFile();
	}

	bool open() override;

	void close() override;

	size_t read(uint8_t *buf, int buf_size) override;
};
