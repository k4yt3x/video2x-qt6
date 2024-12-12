#ifndef FILEDROPTABLEVIEW_H
#define FILEDROPTABLEVIEW_H

#include <QDropEvent>
#include <QMimeData>
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
};

#endif // FILEDROPTABLEVIEW_H
