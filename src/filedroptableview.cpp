#include "filedroptableview.h"

#include <QPainter>

FileDropTableView::FileDropTableView(QWidget *parent)
    : QTableView(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);

    // Initialize the SVG renderer
    m_renderer = new QSvgRenderer(QString(":/images/drag-and-drop.svg"), this);
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
