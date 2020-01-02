
#include "ListViewEx.h"

#define __RemarkAlpha 170

size_t CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pPath)
    {
        return m_paSubDirs.size() + m_paSubFiles.size();
    }
    else
    {
        return _getRootRowCount();
    }
}

void CListViewEx::showRoot()
{
    _clear();

    update();

    _onShowRoot();
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet)
{
    _clear();

    m_pMediaset = &MediaSet;
    m_pMediaset->GetSubSets(m_lstSubSets);
    m_pMediaset->GetMedias(m_lstSubMedias);

    update();

    _onShowMediaSet(MediaSet);
}

void CListViewEx::showMedia(CMedia& media)
{
    if (media.m_pParent)
    {
        showMediaSet(*media.m_pParent);

        int nIdx = media.index();
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
}

void CListViewEx::showPath(CPath& path)
{
    if (!path.fileInfo().bDir)
    {
        auto pParent = path.fileInfo().pParent;
        if (pParent)
        {
            showPath(*pParent);

            int nIdx = pParent->files().indexOf(&path);
            if (nIdx >= 0)
            {
                UINT uIdx = UINT(pParent->dirs().size() + nIdx);
                showRow(uIdx, true);
                selectRow(uIdx);
            }
        }

        return;
    }

    _clear();

    m_pPath = &path;
    m_paSubDirs.assign(path.dirs());
    m_paSubFiles.assign(path.files());

    update();

    _onShowPath(path);
}

void CListViewEx::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet) {
                tagMediaContext context(lvRow, mediaSet);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                tagMediaContext context(lvRow, media);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else if (m_pPath)
    {
        if (lvRow.uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvRow.uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                tagMediaContext context(lvRow, subFile);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            m_paSubDirs.get(lvRow.uRow, [&](CPath& subPath) {
                tagMediaContext context(lvRow, subPath);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else
    {
        tagMediaContext context(lvRow);
        if (_genRootRowContext(context))
        {
            _paintRow(painter, context);
        }
    }
}

void CListViewEx::_paintText(CPainter& painter, QRect& rc, const tagRowContext& context)
{
    cauto mediaContext = (tagMediaContext&)context;

    if (mediaContext.pMediaSet)
    {
        QString qsRemark;
        if (E_MediaSetType::MST_SingerGroup == mediaContext.pMediaSet->m_eType)
        {
            auto pSingerGroup = (CSingerGroup*)mediaContext.pMediaSet;
            qsRemark.sprintf("%u歌手", pSingerGroup->singers().size());
        }
        else if (E_MediaSetType::MST_Singer == mediaContext.pMediaSet->m_eType)
        {
            auto pSinger = (CSinger*)mediaContext.pMediaSet;
            qsRemark.sprintf("%u专辑", pSinger->albums().size());
        }
        else if (E_MediaSetType::MST_Album == mediaContext.pMediaSet->m_eType)
        {
            auto pAlbum = (CAlbum*)mediaContext.pMediaSet;
            qsRemark.sprintf("%u曲目", pAlbum->albumItems().size());
        }
        else if (E_MediaSetType::MST_Playlist == mediaContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mediaContext.pMediaSet;
            qsRemark.sprintf("%u曲目", pPlaylist->playItems().size());
        }

        if (!qsRemark.isEmpty())
        {
            painter.save();

            painter.adjustFont(0.9f);

            UINT uAlpha = oppTextAlpha(__RemarkAlpha);
            painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter
                               , qsRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);

            painter.restore();

            rc.setRight(rc.right() - 100);
        }
    }

    CListView::_paintText(painter, rc, context);
}

void CListViewEx::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me)
{
    UINT uRow = lvRow.uRow;

    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRow, [&](CMediaSet& mediaSet){
                _saveScrollRecord();
                showMediaSet(mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRow, [&](CMedia& media){
                _onRowClick(lvRow, me, media);
            });
        }
    }
    else if (m_pPath)
    {
        if (uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                _onRowClick(lvRow, me, (CPath&)subFile);
            });
        }
        else
        {
            m_paSubDirs.get(uRow, [&](CPath& subPath) {
                _saveScrollRecord();
                showPath(subPath);
            });
        }
    }
    else
    {
        tagMediaContext context(lvRow);
        if (_genRootRowContext(context))
        {
            if (context.pMediaSet)
            {
                _saveScrollRecord();
                showMediaSet(*context.pMediaSet);
            }
            else if (context.pDir)
            {
                _saveScrollRecord();
                showPath(*context.pDir);
            }
        }
    }
}

bool CListViewEx::upward()
{
    _clearScrollRecord(_current());

    if (m_pMediaset)
    {
        auto parent = _onUpward(*m_pMediaset);
        if (parent)
        {
            showMediaSet(*parent);
        }
        else
        {
            showRoot();
        }
    }
    else if (m_pPath)
    {
        auto parent = _onUpward(*m_pPath);
        if (parent)
        {
            showPath(*parent);
        }
        else
        {
            showRoot();
        }
    }
    else
    {
        return false;
    }

    scroll(_scrollRecord(_current()));
    return true;
}
