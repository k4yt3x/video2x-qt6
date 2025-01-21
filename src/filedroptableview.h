#ifndef FILEDROPTABLEVIEW_H
#define FILEDROPTABLEVIEW_H

#include <QDropEvent>
#include <QMimeData>
#include <QSvgRenderer>
#include <QTableView>
#include <QUrl>

class FileDropTableView : public QTableView
{
    Q_OBJECT
public:
    explicit FileDropTableView(QWidget *parent = nullptr);

signals:
    void filesDropped(const QStringList &fileNames);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer *m_renderer = nullptr;
};

#endif // FILEDROPTABLEVIEW_H
