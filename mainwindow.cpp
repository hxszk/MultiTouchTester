
#include <QDesktopWidget>
#include <QPixmap>
#include <QDebug>
#include <QTouchEvent>
#include <QEvent>

#include "mainwindow.h"
#include "touchscene.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(QString("MultiTouchTest_v")
                         .append(QString::number(VERSION_Major))
                         .append(".").append(QString::number(VERSION_Minor)));  

    fullScreenFlag = false;
    this->setFullScreen();

    touchScene = new TouchScene(size().width(), size().height(), this);

    view = new QGraphicsView(this);
    //view->setRenderHint(QPainter::Antialiasing, true);
    view->setScene(touchScene);
    //设置View的边界&填充为0
    view->setStyleSheet("padding:0px;border:0px");

    this->setCentralWidget(view);

    setAttribute(Qt::WA_AcceptTouchEvents);
    installEventFilter(this);

    createActions();

    clearBtn = new QPushButton(tr("Clear"), view);
    exitBtn  = new QPushButton(tr("Exit"), view);
    connect(clearBtn, SIGNAL(clicked()), touchScene, SLOT(cleanPanel()));
    connect(exitBtn, SIGNAL(clicked()), this, SLOT(close()));
    clearBtn->raise();
    exitBtn->raise();
    clearBtn->setStyleSheet("width: 75px;height: 50px;border: 1px solid rgb(100, 100, 100);");
    exitBtn->setStyleSheet("width: 75px;height: 50px;border: 1px solid rgb(100, 100, 100);");
    exitBtn->show();
    pHLayout = new QHBoxLayout(view);
    pHLayout->addStretch();
    pHLayout->addWidget(clearBtn);
    pHLayout->addStretch();
    pHLayout->addWidget(exitBtn);
    pHLayout->addStretch();
    setLayout(pHLayout);

    timerRR = new QTimer(this);
    connect(timerRR, SIGNAL(timeout()), touchScene, SLOT(updateReportRate()));
    timerRR->start(1000);

    connect(this,       SIGNAL(updateTouchPnt(QList<QTouchEvent::TouchPoint>&)),
            touchScene, SLOT(updateTouchData(QList<QTouchEvent::TouchPoint>&)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcut(tr("Esc"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    this->addAction(exitAction);

    fullAction = new QAction(tr("&FullScreen"), this);
    fullAction->setShortcut(tr("F"));
    connect(fullAction, &QAction::triggered, this, &MainWindow::setFullScreen);
    this->addAction(fullAction);

    cleanAction = new QAction(tr("&Clean"), this);
    cleanAction->setShortcut(tr("C"));
    connect(cleanAction, &QAction::triggered, touchScene, &TouchScene::cleanPanel);
    this->addAction(cleanAction);

    showPointAction = new QAction(tr("&ShowPoint"), this);
    showPointAction->setShortcut(tr("P"));
    connect(showPointAction, &QAction::triggered, touchScene, &TouchScene::setShowPnt);
    this->addAction(showPointAction);

    showLineAction = new QAction(tr("&ShowLine"), this);
    showLineAction->setShortcut(tr("L"));
    connect(showLineAction, &QAction::triggered, touchScene, &TouchScene::setShowLine);
    this->addAction(showLineAction);

    showTextAction = new QAction(tr("&ShowText"), this);
    showTextAction->setShortcut(tr("T"));
    connect(showTextAction, &QAction::triggered, touchScene, &TouchScene::setShowText);
    this->addAction(showTextAction);

    switchBgAction = new QAction(tr("Switch BG"), this);
    switchBgAction->setShortcut(tr("SPACE"));
    connect(switchBgAction, &QAction::triggered, touchScene, &TouchScene::switchBG);
    this->addAction(switchBgAction);

    increaseLineSizeAction = new QAction(tr("Increase Line Size"), this);
    increaseLineSizeAction->setShortcut(Qt::Key_Up);
    connect(increaseLineSizeAction, &QAction::triggered, touchScene, &TouchScene::increaseLineSize);
    this->addAction(increaseLineSizeAction);

    decreaseLineSizeAction = new QAction(tr("Decrease Line Size"), this);
    decreaseLineSizeAction->setShortcut(Qt::Key_Down);
    connect(decreaseLineSizeAction, &QAction::triggered, touchScene, &TouchScene::decreaseLineSize);
    this->addAction(decreaseLineSizeAction);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type evType = event->type();
    if (   evType == QEvent::TouchBegin
        || evType == QEvent::TouchUpdate
        || evType == QEvent::TouchEnd)
    {
        QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

/*
        foreach (QTouchEvent::TouchPoint touchPnt, touchPoints) {
            qDebug("id:%x status:%d (%f, %f)",
                   touchPnt.id(),
                   touchPnt.state(),
                   touchPnt.pos().x(), touchPnt.pos().y());
        }
*/
        emit updateTouchPnt(touchPoints);

        return true;
    }
    else
    {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }

}

void MainWindow::setFullScreen()
{
    fullScreenFlag = !fullScreenFlag;

    if(fullScreenFlag == true)
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}
