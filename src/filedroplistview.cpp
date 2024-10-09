#include "filedroplistview.h"

#include <QStringListModel>

FileDropListView::FileDropListView(QWidget *parent)
    : QListView(parent)
{
    this->setModel(new QStringListModel(this));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
}

void FileDropListView::dragEnterEvent(QDragEnterEvent *event)
{
    // Check if the MIME data contains URLs or the "text/uri-list" format
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FileDropListView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FileDropListView::dropEvent(QDropEvent *event)
{
    // Check if the MIME data contains "text/uri-list"
    if (event->mimeData()->hasFormat("text/uri-list")) {
        QStringList fileNames;
        for (const QUrl &url : event->mimeData()->urls()) {
            // Convert URL to local file path and add to the list
            QString localFile = url.toLocalFile();
            if (!localFile.isEmpty()) {
                fileNames << localFile;
            }
        }

        // Emit the signal with the list of file names
        emit filesDropped(fileNames);
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
