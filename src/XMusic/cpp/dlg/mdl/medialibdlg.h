
#pragma once

#include "dlg/dialog.h"

#include "MedialibView.h"

#include "wholeTrackDlg.h"

#include "singerimgdlg.h"

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg();

private:
    CMedialibView m_lv;

    CWholeTrackDlg m_wholeTrackDlg;

    CSingerImgDlg m_singerImgDlg;

    size_t m_uRowCount = 0;

private slots:
    void slot_labelClick(class CLabel*, const QPoint&);

    void slot_playClick();

public:    
    cqpm pmXpk() const
    {
        return m_lv.pmXpk();
    }

    static size_t caleRowCount(int cy);

    size_t rowCount() const
    {
        return m_uRowCount;
    }

    void init();

    void show();

    void showMediaSet(const CMediaSet& MediaSet);
    void showMediaSet(const IMedia& media);

    bool showMedia(const IMedia& media);

    CMediaRes* showLocalFile(cwstr strPath);

    void updateHead(const WString& strTitle);

    void updateSingerImg(CSinger& singer, const tagSingerImg& singerImg);

    bool tryShowWholeTrack(IMedia& media)
    {
        return m_wholeTrackDlg.tryShow(media);
    }

    wstring genAttachTitle(const CSnapshotDir& dir) const
    {
        return m_lv.genAttachTitle(dir);
    }

private:
    void _onClosed() override
    {
        m_lv.cleanup();
    }

    void _relayout(int cx, int cy) override;

    void _relayoutTitle();

    bool _handleReturn() override
    {
        return m_lv.upward();
    }
};
