#include <QShortcut>
#include <QStyle>
#include <QResizeEvent>
#include <QWebFrame>
#include "searchablewebview.h"


SearchableWebView::SearchableWebView(QWidget *parent) :
    QWidget(parent), lineEdit(this), webView(this)
{
    lineEdit.hideOnClear = true;
    lineEdit.hide();
    connect(&lineEdit, &LineEdit::textChanged, [&](const QString& text) {
        // clear selection:
        webView.findText("");
        webView.findText("", QWebPage::HighlightAllOccurrences);
        if(!text.isEmpty()) {
            // select&scroll to one occurence:
            webView.findText(text, QWebPage::FindWrapsAroundDocument);
            // highlight other occurences:
            webView.findText(text, QWebPage::HighlightAllOccurrences);
        }
        // store text for later searches
        searchText = text;
    });

    auto shortcut = new QShortcut(QKeySequence::Find, this);
    connect(shortcut, &QShortcut::activated, [&] {
        lineEdit.show();
        lineEdit.setFocus();
    });

    connect(&webView, &QWebView::loadFinished, [&](bool ok) {
        moveLineEdit();
    });

    connect(&webView, &QWebView::loadStarted, [&]() {
        lineEdit.clear();
    });
}

void SearchableWebView::moveLineEdit() {
    QSize sz = lineEdit.sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    frameWidth += webView.page()->currentFrame()->scrollBarGeometry(Qt::Vertical).width();
    lineEdit.move(rect().right() - frameWidth - sz.width(), rect().top());
    lineEdit.raise();
}

void SearchableWebView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    webView.resize(event->size().width(), event->size().height());
    moveLineEdit();
}

void SearchableWebView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        webView.findText(searchText, QWebPage::FindWrapsAroundDocument);
    }
    if(event->key() == Qt::Key_Escape) {
        lineEdit.clear();
    }
}

void SearchableWebView::load(const QUrl &url) {
    webView.load(url);
}

QSize SearchableWebView::sizeHint() const {
    return webView.sizeHint();
}
