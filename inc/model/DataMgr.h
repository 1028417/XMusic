
#pragma once

class __ModelExt CDataMgr
{
public:
	CDataMgr(CDao& dao)
		: m_dao(dao)
		, m_Option(COptionMgr::inst().getOption())
	{
	}

private:
	CDao& m_dao;

	tagOption& m_Option;

public:
	bool clearData();

	bool updateMediaSetName(CMediaSet& MediaSet, const wstring& strName);
	bool updateMediaSetProperty(CMediaSet& MediaSet, const CMediasetProperty& property);

	void updateMediaSizeDuration(const TD_MediaList& lstMedias);

	bool initAlarmmedia();
	
	bool addAlarmmedia(const TD_IMediaList& lstMedias);
	
	bool removeAlarmmedia(UINT uIndex);

	bool clearAlarmmedia();

	bool queryPlayRecordMaxTime(dbtime_t& tTime);
	bool queryPlayRecord(dbtime_t tTime, vector<pair<wstring, int>>& vecPlayRecord);
	bool clearPlayRecord();
};
