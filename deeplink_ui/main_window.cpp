#include "main_window.h"

#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "bridge.h"
#include "devtools_window.h"
#include "webview.h"

MainWindow::MainWindow(bool for_dev_tools)
    : QMainWindow(nullptr)
    , bridge_(nullptr)
    , view_(nullptr)
    , profile_(nullptr)
    , url_edit_(nullptr)
    , progress_bar_(nullptr)
    , dev_tools_(nullptr)
{
    if (!for_dev_tools)
        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    // setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    if (for_dev_tools) {
        QHBoxLayout *dev_layout = new QHBoxLayout;
        dev_layout->setSpacing(0);
        dev_layout->setContentsMargins(5, 5, 5, 5);

        url_edit_ = new QLineEdit(this);
        url_edit_->setObjectName("leUrl");
        url_edit_->setFixedWidth(500);
        // url_edit_->setMaximumWidth(600);
        connect(url_edit_, &QLineEdit::returnPressed, this, [this]() {
            if (view_) view_->setUrl(QUrl::fromUserInput(url_edit_->text()));
        });
        dev_layout->addWidget(url_edit_);

        QPushButton *pbtn_reload = new QPushButton(this);
        pbtn_reload->setText(tr("Reload"));
        pbtn_reload->setFixedWidth(72);
        connect(pbtn_reload, &QPushButton::clicked, this, [this](bool checked) {
            if (view_) view_->reload();
        });
        dev_layout->addSpacing(10);
        dev_layout->addWidget(pbtn_reload);

        QPushButton *pbtn_dev_tools = new QPushButton(this);
        pbtn_dev_tools->setText(tr("Dev Tools"));
        pbtn_dev_tools->setFixedWidth(72);
        connect(pbtn_dev_tools, &QPushButton::clicked, this, &MainWindow::pbtnDevToolsClicked);
        dev_layout->addSpacing(10);
        dev_layout->addWidget(pbtn_dev_tools, 0, Qt::AlignLeft);

        QLabel *lbl_input = new QLabel("Input:", this);
        lbl_input->setMaximumHeight(16);
        dev_layout->addWidget(lbl_input, 0, Qt::AlignRight);
        QLineEdit *le_input = new QLineEdit(this);
        le_input->setObjectName("leInput");
        le_input->setFixedWidth(100);
        dev_layout->addSpacing(10);
        dev_layout->addWidget(le_input);
        QLabel *lbl_output = new QLabel("", this);
        lbl_output->setObjectName("lblOutput");
        lbl_output->setFixedWidth(100);
        lbl_output->setMaximumHeight(16);
        dev_layout->addSpacing(10);
        dev_layout->addWidget(lbl_output);
        QPushButton *pbtn_call_js_func = new QPushButton("Call JS Func", this);
        pbtn_call_js_func->setFixedWidth(100);
        connect(pbtn_call_js_func, &QPushButton::clicked, this, &MainWindow::pbtnCallJsFuncClicked);
        dev_layout->addSpacing(10);
        dev_layout->addWidget(pbtn_call_js_func);
        QLabel *lbl_time = new QLabel(this);
        lbl_time->setObjectName("lblTime");
        lbl_time->setMaximumHeight(16);
        dev_layout->addSpacing(10);
        dev_layout->addWidget(lbl_time);

        layout->addLayout(dev_layout);

        progress_bar_ = new QProgressBar(this);
        progress_bar_->setMaximum(100);
        progress_bar_->setMinimum(0);
        progress_bar_->setMaximumHeight(2);
        progress_bar_->setMinimumHeight(2);
        progress_bar_->setTextVisible(false);
        layout->addWidget(progress_bar_);
    }

    bridge_ = new Bridge(this);
    connect(bridge_, &Bridge::sig_showMinimized, this, &MainWindow::bridgeShowMinisized);
    connect(bridge_, &Bridge::sig_closeWindow, this, &MainWindow::bridgeCloseWindow);

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("doc", bridge_);
    view_ = new WebView(this);
    // view->page()->setWebChannel(channel);
    connect(view_, &QWebEngineView::loadStarted, this, &MainWindow::webViewLoadStarted);
    connect(view_, &QWebEngineView::urlChanged, this, &MainWindow::webViewUrlChanged);
    connect(view_, &QWebEngineView::loadProgress, this, &MainWindow::webViewLoadProgress);
    connect(view_, &QWebEngineView::loadFinished, this, &MainWindow::webViewLoadFinished);

    profile_ = new QWebEngineProfile(
        QString::fromLatin1("deeplinkbrowser.%1").arg(qWebEngineChromiumVersion()), this);
    profile_->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    profile_->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    profile_->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    profile_->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    // profile_->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    QWebEnginePage *web_page = new QWebEnginePage(profile_, view_);
    web_page->setWebChannel(channel);
    view_->setPage(web_page);

    layout->addWidget(view_);
    central_widget->setLayout(layout);
    setCentralWidget(central_widget);

    view_->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setUrl(const QString &url)
{
    view_->setUrl(QUrl::fromUserInput(url));
    if (url_edit_) url_edit_->setText(url);
}

void MainWindow::webViewLoadStarted()
{
    QPushButton *btn = findChild<QPushButton*>("pbtnReload");
    if (btn) btn->setEnabled(false);
}

void MainWindow::webViewLoadFinished(bool ok)
{
    QPushButton *btn = findChild<QPushButton*>("pbtnReload");
    if (btn) btn->setEnabled(true);
}

void MainWindow::webViewUrlChanged(const QUrl &url)
{
    if (url_edit_)
        url_edit_->setText(url.toDisplayString());
}

void MainWindow::webViewLoadProgress(int progress)
{
    if (progress_bar_)
        progress_bar_->setValue(progress);
}

void MainWindow::pbtnDevToolsClicked()
{
    if (!dev_tools_) {
        dev_tools_ = new DevToolsWindow(profile_, nullptr);
        QObject::connect(dev_tools_, &QObject::destroyed, this, [this](QObject *) {
            dev_tools_ = nullptr;
        });
        view_->page()->setDevToolsPage(dev_tools_->webView()->page());
        view_->page()->triggerAction(QWebEnginePage::InspectElement);
    }
    dev_tools_->show();
}

void MainWindow::pbtnCallJsFuncClicked()
{
    QLineEdit *le_input = findChild<QLineEdit*>("leInput");
    QString str_input = le_input->text();
    QLabel *lbl_output = findChild<QLabel*>("lblOutput");
    if (str_input.isEmpty()) {
        lbl_output->setText("input is empty");
    } else {
        // QTime t1 = QTime::currentTime();
        QElapsedTimer t1;
        t1.start();
        view_->page()->runJavaScript("document.title", [](const QVariant &v) {
            qDebug() << "run js document.title {}" << v.toString();
        });
        // QTime t2 = QTime::currentTime();
        float interval1 = t1.nsecsElapsed() * 1e-6;
        t1.restart();
        view_->page()->runJavaScript(
                QString("CallByCPP(\"%1\")").arg(str_input),
                [lbl_output](const QVariant &v) {
                        lbl_output->setText(v.toString());
                }
        );
        // qint64 interval1 = t1.msecsTo(t2);
        float interval2 = t1.nsecsElapsed() * 1e-6;
        QLabel *lbl_time = findChild<QLabel*>("lblTime");
        if (lbl_time)
            lbl_time->setText(QString("%1ms, %2ms").arg(interval1).arg(interval2));
    }
}

void MainWindow::bridgeShowMinisized()
{
    this->showMinimized();
}

void MainWindow::bridgeCloseWindow()
{
    // this->close();
    this->hide();
}
