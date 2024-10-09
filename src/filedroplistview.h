#ifndef FILEDROPLISTVIEW_H
#define FILEDROPLISTVIEW_H

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QListView>
#include <QMimeData>
#include <QUrl>

class FileDropListView : public QListView
{
    Q_OBJECT

public:
    explicit FileDropListView(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void filesDropped(const QStringList &fileNames);
};

#endif // FILEDROPLISTVIEW_H
