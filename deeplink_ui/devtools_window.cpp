#include "devtools_window.h"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>

#include "webview.h"

DevToolsWindow::DevToolsWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow{parent}
    , view_(new WebView(this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(view_);
    central_widget->setLayout(layout);
    setCentralWidget(central_widget);

    view_->setPage(new QWebEnginePage(profile, view_));

    connect(view_, &WebView::titleChanged, this, &DevToolsWindow::handleWebViewTitleChanged);
    connect(view_->page(), &QWebEnginePage::windowCloseRequested, this, &QWidget::close);

    // view->resize()
    view_->show();
}

QSize DevToolsWindow::sizeHint() const
{
    QRect desktop_rect = QApplication::primaryScreen()->geometry();
    QSize size = desktop_rect.size() * qreal(0.9);
    return size;
}

void DevToolsWindow::handleWebViewTitleChanged(const QString &title)
{
    setWindowTitle(title);
}

WebView *DevToolsWindow::webView() const
{
    return view_;
}
