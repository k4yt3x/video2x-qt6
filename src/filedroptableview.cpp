#include "filedroptableview.h"

FileDropTableView::FileDropTableView(QWidget *parent)
    : QTableView(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    // If you need a model, set it externally:
    // setModel(yourModel);
}

void FileDropTableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FileDropTableView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FileDropTableView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        QStringList fileNames;
        for (const QUrl &url : event->mimeData()->urls()) {
            QString localFile = url.toLocalFile();
            if (!localFile.isEmpty()) {
                fileNames << localFile;
            }
        }

        emit filesDropped(fileNames);
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
