
#pragma once

#define __CNLanguage		0x01
#define __HKLanguage		0x02
#define __KRLanguage		0x04
#define __JPLanguage		0x08
#define __TAILanguage	    0x10
#define __ENLanguage		0x20
#define __EURLanguage	    0x40

enum class E_LanguageType
{
	LT_None = 0,
	LT_CN = __CNLanguage,
	LT_HK = __HKLanguage,
	LT_KR = __KRLanguage,
	LT_JP = __JPLanguage,
	LT_TAI = __TAILanguage,
	LT_EN = __ENLanguage,
	LT_EUR = __EURLanguage,
};

class CMediasetProperty
{
public:
	CMediasetProperty(UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport = false)
		: m_uLanguage(uLanguage)
		, m_bDisableDemand(bDisableDemand)
		, m_bDisableExport(bDisableExport)
	{
	}
	
private:
	UINT m_uLanguage = 0;
	bool m_bDisableDemand = false;
	bool m_bDisableExport = false;

public:
	UINT language() const
	{
		return m_uLanguage;
	}
	bool isDisableDemand() const
	{
		return m_bDisableDemand;
	}
	bool isDisableExport() const
	{
		return m_bDisableExport;
	}

	void setDisableDemand(bool bDisableDemand)
	{
		m_bDisableDemand = bDisableDemand;
	}
	void setDisableExport(bool bDisableExport)
	{
		m_bDisableExport = bDisableExport;
	}
	void setLanguage(E_LanguageType eLanguageType)
	{
		m_uLanguage |= (UINT)eLanguageType;
	}
	void unsetLanguage(E_LanguageType eLanguageType)
	{
		m_uLanguage &= ~(UINT)eLanguageType;
	}

	bool checkLanguage(E_LanguageType eLanguageType) const
	{
		return m_uLanguage & (UINT)eLanguageType;
	}

	bool isCNLanguage() const
	{
		return m_uLanguage & __CNLanguage;
	}
	bool isHKLanguage() const
	{
		return m_uLanguage & __HKLanguage;
	}
	bool isKRLanguage() const
	{
		return m_uLanguage & __KRLanguage;
	}
	bool isJPLanguage() const
	{
		return m_uLanguage & __JPLanguage;
	}
	bool isTAILanguage() const
	{
		return m_uLanguage & __TAILanguage;
	}
	bool isENLanguage() const
	{
		return m_uLanguage & __ENLanguage;
	}
	bool isEURLanguage() const
	{
		return m_uLanguage & __EURLanguage;
	}
};

class __MediaLibExt CMediaSet : public tagMediaInfo, public CListObject, public CTreeObject
{
public:
    CMediaSet(const wstring& strName = L"", CMediaSet *pParent = NULL, int nID = 0
            , E_MediaSetType eType = E_MediaSetType::MST_Null
            , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false)
		: tagMediaInfo(pParent, strName, nID)
		, m_eType(eType)
		, m_property(uLanguage, bDisableDemand, bDisableExport)
    {
	}
	
public:
	E_MediaSetType m_eType;

private:
	CMediasetProperty m_property;

private:
	virtual int _indexOf(const CMediaSet&) const { return -1; }

	bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = m_strName;
		return true;
	}

	wstring GetTreeText() const override
	{
		return m_strName;
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds) override;

	bool hasCheckState() override
	{
		return available();
	}

	bool _FindMedia(CMedia& media, const tagFindMediaPara& FindPara, struct tagFindMediaResult& FindResult);

public:
	const CMediasetProperty& property() const
	{
		return m_property;
	}

	void setProperty(const CMediasetProperty& property)
	{
		m_property = property;
	}

	bool checkLanguage(E_LanguageType eLanguageType) const;

	int index() const
	{
		if (m_pParent)
		{
			return m_pParent->_indexOf(*this);
		}

		return -1;
	}

	virtual int indexOf(const CMedia&) const { return -1; }

	virtual wstring GetBaseDir() const;

    virtual void GetMedias(TD_MediaList& lstMedias) {(void)lstMedias;}

    virtual void GetSubSets(TD_MediaSetList& lstSubSets) const {(void)lstSubSets;}

    virtual bool available() const;

	void GetAllMediaSets(E_MediaSetType eType, TD_MediaSetList& arrMediaSets);
	void GetAllMediaSets(TD_MediaSetList& arrMediaSets);

	void GetAllMedias(TD_MediaList& lstMedias);

    CMediaSet* FindSubSet(E_MediaSetType eMediaSetType, UINT uMediaSetID);

    CMedia* FindMedia(E_MediaSetType eMediaSetType, UINT uMediaID);

	wstring GetLogicPath();

	virtual wstring GetExportName()
	{
		return m_strName;
	}

	virtual bool FindMedia(const tagFindMediaPara& FindPara, struct tagFindMediaResult& FindResult);
};

struct tagFindMediaResult
{
	UINT uResult = 0;

	TD_MediaList lstRetMedias;

	CMediaSet *pRetSinger = NULL;

	map<UINT, wstring> mapSingerDirChanged;
};
