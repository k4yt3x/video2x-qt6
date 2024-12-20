#ifndef QTTEXTEDITSINK_H
#define QTTEXTEDITSINK_H

#include <mutex>

#include <QObject>
#include <QTextEdit>

#include <spdlog/sinks/base_sink.h>

class QtTextEmitter : public QObject
{
    Q_OBJECT
signals:
    void appendText(const QString &text);
};

class QtTextEditSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    explicit QtTextEditSink();
    ~QtTextEditSink();
    void setTextEdit(QTextEdit *textEdit);

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;
    void flush_() override;

private:
    QTextEdit *textEdit_;
    QtTextEmitter *emitter_;
};

#endif // QTTEXTEDITSINK_H
