
#include "view.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

#include "listview.h"

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

CMedialibDlg::CMedialibDlg(class CPlayerView& view, QWidget *parent) :
    CDialog(parent)
    , m_view(view)
    , m_MediaLib(view.getModel())
{
    ui.setupUi(this);
}
