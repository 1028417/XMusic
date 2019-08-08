#include "medialibdlg.h"
#include "ui_medialibdlg.h"

#include "listview.h"

class CMediaLib : public CMediaSet
{

};

class CMedialibView : public CListView
{
public:
    CMedialibView(class CPlayerView& view, QWidget *parent=NULL) :
        CListView(parent)
        , m_view(view)
    {
    }

private:
    class CPlayerView& m_view;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;

    CMediaRes *m_pMediaRes = NULL;

private:
    UINT getItemCount() override
    {
        return 0;
    }

    virtual void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) override
    {

    }
};

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(QWidget *parent) :
    CDialog(parent)
{
    ui.setupUi(this);
}
