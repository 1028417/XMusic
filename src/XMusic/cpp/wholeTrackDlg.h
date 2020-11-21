
#pragma once

#include "dialog.h"

class CWholeTrackView : public CListView
{
public:
    CWholeTrackView(class CWholeTrackDlg& WholeTrackDlg);

private:
    class CWholeTrackDlg& m_WholeTrackDlg;

    CCueFile m_cue;
    UINT m_uDuration = 0;

public:
    void setCue(CCueFile cue, UINT uDuration);

private:
    size_t getColCount() const override;
    size_t getRowCount() const override;
    size_t getItemCount() const override;

    cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;
};

class CWholeTrackDlg : public CDialog
{
public:
    CWholeTrackDlg(class CMedialibDlg& medialibDlg);

private:
    IMedia *m_pMedia = NULL;

    CWholeTrackView m_lv;

public:
    void relayout(cqrc rcBtnReturn, cqrc rcLabelDisk, cqrc rcTitle, cqrc rcBtnPlay, cqrc rcLv);

    bool tryShow(IMedia& media);
};
