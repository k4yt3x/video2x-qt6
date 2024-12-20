#include "qttexteditsink.h"

QtTextEditSink::QtTextEditSink()
    : textEdit_(nullptr)
    , emitter_(new QtTextEmitter())
{}

QtTextEditSink::~QtTextEditSink()
{
    delete emitter_;
}

void QtTextEditSink::setTextEdit(QTextEdit *textEdit)
{
    std::lock_guard<std::mutex> lock(mutex_);
    textEdit_ = textEdit;

    if (textEdit_) {
        QObject::connect(emitter_,
                         &QtTextEmitter::appendText,
                         textEdit_,
                         &QTextEdit::append,
                         Qt::QueuedConnection);
    }
}

void QtTextEditSink::sink_it_(const spdlog::details::log_msg &msg)
{
    if (textEdit_ == nullptr) {
        return;
    }

    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);
    QString text = QString::fromUtf8(formatted.data(), static_cast<int>(formatted.size()));

    // Chop the trailing newline if it exists
    if (text.endsWith('\n')) {
        text.chop(1);
    }

    // Append text directly using QTextCursor to avoid extra newlines
    QMetaObject::invokeMethod(
        textEdit_,
        [textEdit = textEdit_, text]() {
            QTextCursor cursor = textEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.insertText(text);
            textEdit->setTextCursor(cursor);
        },
        Qt::QueuedConnection);
}

void QtTextEditSink::flush_() {}
