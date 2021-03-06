﻿
#pragma once

#define __CnLanguage		0x01
#define __HkLanguage		0x02
#define __KrLanguage		0x04
#define __JpLanguage		0x08
#define __EnLanguage		0x10

// 小语种
#define __OtherLanguage	    0x20
#define __TlLanguage	    0x120
#define __RsLanguage	    0x220
#define __FrLanguage	    0x420

enum class E_LanguageType
{
	LT_None = 0,

    LT_CN = __CnLanguage,
    LT_HK = __HkLanguage,
    LT_KR = __KrLanguage,
    LT_JP = __JpLanguage,
    LT_EN = __EnLanguage,

    // 小语种
    LT_Other = __OtherLanguage,
    LT_TL = __TlLanguage,
    LT_RS = __RsLanguage,
    LT_FR = __FrLanguage,
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

#if __winvc
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
#endif

    bool isCnLanguage() const
	{
        return m_uLanguage & __CnLanguage;
	}
    bool isHkLanguage() const
	{
        return m_uLanguage & __HkLanguage;
	}
    bool isKrLanguage() const
	{
        return m_uLanguage & __KrLanguage;
	}
    bool isJpLanguage() const
	{
        return m_uLanguage & __JpLanguage;
	}
    bool isEnLanguage() const
	{
        return m_uLanguage & __EnLanguage;
	}

    bool isOtherLanguage() const
    {
        return m_uLanguage & __OtherLanguage;
    }
    bool isTlLanguage() const
    {
        return (m_uLanguage & __TlLanguage) == __TlLanguage;
    }
    bool isRsLanguage() const
    {
        return (m_uLanguage & __RsLanguage) == __RsLanguage;
    }
    bool isFrLanguage() const
    {
        return (m_uLanguage & __FrLanguage) == __FrLanguage;
    }
};

class __MediaLibExt CMediaSet : public tagMediaInfo
#if __winvc
	, public CListObject, public CTreeObject
#endif
{
public:
    CMediaSet(cwstr strName = L"", CMediaSet *pParent = NULL
            , E_MediaSetType eType = E_MediaSetType::MST_None, UINT uID = 0
            , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false)
        : tagMediaInfo(pParent, strName, uID)
		, m_eType(eType)
		, m_property(uLanguage, bDisableDemand, bDisableExport)
    {
	}

#if !__winvc
    CMediaSet(CMediaSet *pParent, E_MediaSetType eType) //CSnapshotDir构造提速
        : m_eType(eType)
    {
        m_pParent = pParent;
    }

    bool m_bAvailable = false;
    virtual bool available() const
    {
        return m_bAvailable;
    }
#endif

protected:
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
#endif

public:    
    E_MediaSetType type() const
    {
        return m_eType;
    }

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

	void GetAllMediaSets(E_MediaSetType eType, TD_MediaSetList& arrMediaSets);
	void GetAllMediaSets(TD_MediaSetList& arrMediaSets);

    virtual void GetAllMedias(TD_IMediaList& lstMedias);

    CMediaSet* GetSubSet(E_MediaSetType eMediaSetType, UINT uMediaSetID);

	wstring GetLogicPath();

	virtual wstring GetExportName()
	{
		return m_strName;
    }
};
