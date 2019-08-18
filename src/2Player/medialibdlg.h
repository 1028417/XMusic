
#pragma once

#include "model.h"

#include "dialog.h"

#include "listview.h"

#include "button.h"

enum class E_ItemStyle
{
    IS_Normal
    , IS_Underline
    , IS_RightTip
};

struct tagItemContext
{
    E_ItemStyle eStyle = E_ItemStyle::IS_Normal;

    QPixmap *pixmap = NULL;

    wstring strText;

    wstring strRemark;

    UINT uIconSize = 0;
};

struct tagRootItemContext : tagItemContext
{
    CMediaSet *pMediaSet = NULL;
    CMediaRes *pMediaRes = NULL;
};

class CListViewEx : public CListView
{
public:
    CListViewEx(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0);

protected:
    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;

private:
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    map<void*, float> m_mapScrollRecord;

    QPixmap m_pmRightTip;

private:
    UINT getRowCount() override;

    virtual UINT getRootCount() = 0;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    virtual bool _genRootRowContext(const tagLVRow&, tagRootItemContext&) = 0;

    virtual bool _genRowContext(CMediaSet&, tagItemContext&) {return false;}
    virtual bool _genRowContext(CMedia&, tagItemContext&) {return false;}
    virtual bool _genRowContext(CMediaRes&, tagItemContext&) {return false;}

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
    virtual void _onRowClick(const tagLVRow&, IMedia&){}

protected:
    virtual void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagItemContext&);

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pMediaRes;
    }

public:
    virtual void showRoot();

    virtual void showMediaSet(CMediaSet&, CMedia *pHittestItem=NULL);

    virtual void showMediaRes(CMediaRes&);

    bool handleReturn();
};

class CMedialibView : public CListViewEx
{
public:
    CMedialibView(class CPlayerView& view, class CMedialibDlg& medialibDlg);

private:
    class CPlayerView& m_view;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaRes& m_RootMediaRes;

    CMediaRes m_sdcard;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmDefaultSinger;
    QPixmap m_pmAlbum;
    QPixmap m_pmAlbumItem;

    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmDir;
    QPixmap m_pmDirLink;
    QPixmap m_pmFile;

public:
    void init();

    void showRoot() override;

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL) override;

    void showMediaRes(CMediaRes& MediaRes) override;

    void showMediaRes(const wstring& strPath);

    bool handleReturn();

private:
    UINT getPageRowCount() override;

    UINT getColumnCount() override;

    UINT getRootCount() override;

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaRes&, WString& strTitle);

    bool _genRootRowContext(const tagLVRow&, tagRootItemContext&) override;

    bool _genRowContext(CMediaSet&, tagItemContext&) override;
    bool _genRowContext(CMedia&, tagItemContext&) override;
    bool _genRowContext(CMediaRes&, tagItemContext&) override;

    void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagItemContext&) override;

    QPixmap& _getSingerPixmap(CSinger&);

    void _onRowClick(const tagLVRow&, IMedia&) override;
};

class CMedialibDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CMedialibDlg(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    CMedialibView m_MedialibView;

public:
    void init();

    void show()
    {
        CDialog<>::show();
        m_MedialibView.showRoot();
    }

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestMedia=NULL)
    {
        CDialog<>::show();
        m_MedialibView.showMediaSet(MediaSet, pHittestMedia);
    }

    void showMediaRes(const wstring& strPath)
    {
        CDialog<>::show();
        m_MedialibView.showMediaRes(strPath);
    }

    void showUpwardButton(bool bVisible) const;

    void setTitle(const wstring& strTitle) const;

private:
    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.handleReturn();
    }
};
