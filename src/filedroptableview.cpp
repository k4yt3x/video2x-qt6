#include "filedroptableview.h"

#include <QCollator>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QPainter>

#include <algorithm>

FileDropTableView::FileDropTableView(QWidget *parent)
    : QTableView(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDragDropOverwriteMode(false);
    setDefaultDropAction(Qt::MoveAction);

    // Initialize the SVG renderer
    m_renderer = new QSvgRenderer(QString(":/images/drag-and-drop.svg"), this);
}

void FileDropTableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->source() == this) {
        QTableView::dragEnterEvent(event);
    } else if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FileDropTableView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->source() == this) {
        QTableView::dragMoveEvent(event);
    } else if (event->mimeData()->hasFormat("text/uri-list")) {
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

        QCollator collator;
        collator.setNumericMode(true);
        collator.setCaseSensitivity(Qt::CaseInsensitive);
        std::sort(fileNames.begin(), fileNames.end(), [&collator](const QString &left,
                                                                  const QString &right) {
            const QString leftFileName = QFileInfo(left).fileName();
            const QString rightFileName = QFileInfo(right).fileName();

            const int fileNameCompare = collator.compare(leftFileName, rightFileName);
            if (fileNameCompare != 0) {
                return fileNameCompare < 0;
            }

            const int caseCompare = QString::compare(leftFileName, rightFileName, Qt::CaseSensitive);
            if (caseCompare != 0) {
                return caseCompare < 0;
            }

            return QString::compare(left, right, Qt::CaseSensitive) < 0;
        });

        emit filesDropped(fileNames);
        event->acceptProposedAction();
    } else if (event->source() == this) {
        if (model() == nullptr || selectionModel() == nullptr) {
            event->ignore();
            return;
        }

        const QModelIndexList rows = selectionModel()->selectedRows();
        if (rows.size() != 1) {
            event->ignore();
            return;
        }

        const int sourceRow = rows.first().row();
        const QModelIndex dropIndex = indexAt(event->position().toPoint());
        int destinationRow = model()->rowCount();

        switch (dropIndicatorPosition()) {
        case QAbstractItemView::AboveItem:
        case QAbstractItemView::OnItem:
            destinationRow = dropIndex.row();
            break;
        case QAbstractItemView::BelowItem:
            destinationRow = dropIndex.row() + 1;
            break;
        case QAbstractItemView::OnViewport:
            break;
        }

        if (destinationRow < 0 || destinationRow > model()->rowCount()) {
            event->ignore();
            return;
        }

        if (destinationRow == sourceRow || destinationRow == sourceRow + 1) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
            return;
        }

        emit rowMoveRequested(sourceRow, destinationRow);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void FileDropTableView::paintEvent(QPaintEvent *event)
{
    QTableView::paintEvent(event);

    // Check if the table is empty
    if (model() != nullptr && model()->rowCount() != 0) {
        return;
    }

    // Check if the SVG renderer is valid
    if (!m_renderer->isValid()) {
        return;
    }

    QPainter painter(viewport());

    // Define the SVG image's size
    QSize svgSize(134, 125);

    // Calculate the center of the table
    QPoint svgCenter(rect().width() / 2, rect().height() / 2);

    // Position the SVG rectangle
    QRect svgRect(svgCenter.x() - svgSize.width() / 2,
                  svgCenter.y() - svgSize.height() / 2 - 20,
                  svgSize.width(),
                  svgSize.height());

    // Render the SVG
    m_renderer->render(&painter, svgRect);

    // Set up the font for the text
    QFont font = painter.font();
    font.setPointSize(18);
    painter.setFont(font);
    painter.setPen(Qt::darkGray);

    // Position the text rectangle below the SVG
    QRect textRect(0, svgRect.bottom() + 10, viewport()->width(), 50);

    // Draw the text
    painter.drawText(textRect,
                     Qt::AlignTop | Qt::AlignHCenter,
                     tr("Drop files here to add new processing tasks"));
}
