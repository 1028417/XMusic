
#pragma once

#include "dialog.h"

class CWholeTrackView : public CListView
{
public:
    CWholeTrackView(class CWholeTrackDlg& WholeTrackDlg);

private:
    class CWholeTrackDlg& m_WholeTrackDlg;

    CCueFile m_cue;

public:
    void showCue(CCueFile cue);

private:
    size_t getPageRowCount() const override;
    size_t getRowCount() const override;

    void _onPaintRow(CPainter& painter, tagLVRow& lvRow) override;

    cqrc _paintText(const tagRowContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;
};

class CWholeTrackDlg : public CDialog
{
public:
    CWholeTrackDlg(class CMedialibDlg& medialibDlg, class CApp& app);

private:
    class CMedialibDlg& m_medialibDlg;

    class CApp& m_app;

    class CMediaRes *m_pMediaRes;

    CWholeTrackView m_lv;

public:
    void relayout(cqrc rcBtnReturn, cqrc rcBtnPlay, cqrc rcLv);

    bool tryShow(class CMediaRes& mediaRes);
};
