
#pragma once

class __ModelExt CDataMgr
{
public:
	CDataMgr(CDao& dao, tagOption& Option)
		: m_dao(dao)
		, m_Option(Option)
	{
	}

private:
	CDao& m_dao;

	tagOption& m_Option;

private:
    bool _syncMediaLib(const CByteBuffer& bbfData);

    bool _upgradeMediaLib(UINT uCurrVer, const CCharBuffer& cbfConf);

public:
	bool upgradeMediaLib(const wstring& strDBFile);

	bool clearData();

	bool updateMediaSetName(CMediaSet& MediaSet, const wstring& strName);
	bool updateMediaSetProperty(CMediaSet& MediaSet, const CMediasetProperty& property);
	
	bool initAlarmmedia();
	
	bool addAlarmmedia(const TD_IMediaList& lstMedias);
	
	bool removeAlarmmedia(UINT uIndex);

	bool clearAlarmmedia();

	bool queryPlayRecordMaxTime(dbtime_t& tTime);
	bool queryPlayRecord(dbtime_t tTime, vector<pair<wstring, int>>& vecPlayRecord);
	bool clearPlayRecord();
};
