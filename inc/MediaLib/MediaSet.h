
#pragma once

#define __CNLanguage		0x01
#define __HKLanguage		0x02
#define __KRLanguage		0x04
#define __JPLanguage		0x08
#define __ENLanguage		0x20
#define __EURLanguage	    0x40

enum class E_LanguageType
{
	LT_None = 0,
	LT_CN = __CNLanguage,
	LT_HK = __HKLanguage,
	LT_KR = __KRLanguage,
    LT_JP = __JPLanguage,
	LT_EN = __ENLanguage,
	LT_EUR = __EURLanguage,
};

class __MediaLibExt CMediasetProperty
{
public:
    static cwstr languageName(UINT uLanguage);
    static cwstr languageName(E_LanguageType eLanguage);

    CMediasetProperty() = default;

    CMediasetProperty(UINT uLanguage, bool bDisableDemand, bool bDisableExport)
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
	bool isENLanguage() const
	{
		return m_uLanguage & __ENLanguage;
	}
	bool isEURLanguage() const
	{
		return m_uLanguage & __EURLanguage;
	}
};

class __MediaLibExt CMediaSet : public tagMediaInfo
#if __winvc
	, public CListObject, public CTreeObject
#endif
{
public:
    CMediaSet(cwstr strName = L"", CMediaSet *pParent = NULL, int nID = 0
            , E_MediaSetType eType = E_MediaSetType::MST_None
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

#if __winvc
	virtual bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = m_strName;
		return true;
	}

	virtual wstring GetTreeText() const override
	{
		return m_strName;
	}

	virtual void GetTreeChilds(TD_TreeObjectList& lstChilds) override;

	bool hasCheckState() override
	{
		return playable();
	}
#endif

public:
    void setProperty(const CMediasetProperty& property)
    {
        m_property = property;
    }

	const CMediasetProperty& property() const
	{
		return m_property;
	}

    UINT language() const;

    cwstr languageName() const
    {
        return CMediasetProperty::languageName(language());
    }

    bool isDisableDemand() const;

    bool isDisableExport() const;

	int index() const
	{
		if (m_pParent)
		{
			return m_pParent->_indexOf(*this);
		}

		return -1;
	}

    virtual int indexOf(const IMedia&) const { return -1; }

    virtual void GetMedias(TD_IMediaList&) {}

    virtual void GetSubSets(TD_MediaSetList&) {}

    virtual bool playable() const;

	void GetAllMediaSets(E_MediaSetType eType, TD_MediaSetList& arrMediaSets);
	void GetAllMediaSets(TD_MediaSetList& arrMediaSets);

    void GetAllMedias(TD_IMediaList& lstMedias);

    CMediaSet* GetSubSet(E_MediaSetType eMediaSetType, UINT uMediaSetID);

	wstring GetLogicPath();

	virtual wstring GetExportName()
	{
		return m_strName;
	}

#if !__winvc
    virtual wstring GetDisplayName() const
    {
        return m_strName;
    }
#endif
};
