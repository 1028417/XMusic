
#pragma once

#include "dialog.h"

#include "model.h"

class CMediaSetEx : public CMediaSet
{
public:
    CMediaSetEx(CMediaRes& MediaRes)
        : m_MediaRes(MediaRes)
    {
    }

    CMediaRes& m_MediaRes;
};

class CMediaLib : public CMediaSet
{
public:
    CMediaLib(IModel& model)
        : m_model(model)
        , m_RootMediaRes(model.getRootMediaRes())
    {
    }

private:
    IModel& m_model;

    CMediaSetEx m_RootMediaRes;

    virtual void GetSubSets(TD_MediaSetList& lstSubSets)
    {
        lstSubSets.add(m_RootMediaRes);

        lstSubSets.add(m_model.getPlaylistMgr());
        lstSubSets.add(m_model.getSingerMgr());
    }
};

class CMedialibDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CMedialibDlg(class CPlayerView& view, QWidget *parent = 0);

private:
    class CPlayerView& m_view;

    CMediaLib m_MediaLib;
};
