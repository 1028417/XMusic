
#pragma once

class __ModelExt CDataMgr
{
public:
    CDataMgr() = default;

private:
    vector<wstring> m_vctAlarmmedia;

public:
	bool clearData();

	bool updateMediaSetName(CMediaSet& MediaSet, cwstr strName);
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
	bool queryPlayRecord(dbtime_t tTime, prvector<wstring, int>& vecPlayRecord);
	bool clearPlayRecord();
};
