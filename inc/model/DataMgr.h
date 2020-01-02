
#pragma once

class __ModelExt CDataMgr
{
public:
	CDataMgr(CDao& dao, tagOption& Option)
        : m_dao(dao)
	{
	}

private:
	CDao& m_dao;

    vector<wstring> m_vctAlarmmedia;

public:
	bool clearData();

	bool updateMediaSetName(CMediaSet& MediaSet, const wstring& strName);
	bool updateMediaSetProperty(CMediaSet& MediaSet, const CMediasetProperty& property);

	void updateMediaSizeDuration(const TD_MediaList& lstMedias);

	bool initAlarmmedia();
	
	bool addAlarmmedia(const TD_IMediaList& lstMedias);
	
	bool removeAlarmmedia(UINT uIndex);

	bool clearAlarmmedia();

    const vector<wstring>& alarmmedias() const
    {
        return m_vctAlarmmedia;
    }

	bool queryPlayRecordMaxTime(dbtime_t& tTime);
	bool queryPlayRecord(dbtime_t tTime, vector<pair<wstring, int>>& vecPlayRecord);
	bool clearPlayRecord();
};
