#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QApplication>
#include <QAction>
#include <QTouchEvent>
#include <QTimer>
#include <QPushButton>
#include <QHBoxLayout>

#include "touchscene.h"

#define VERSION_Major   0x00
#define VERSION_Minor   0x01

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void updateTouchPnt(QList<QTouchEvent::TouchPoint> &touchPnts);

public slots:
    void setFullScreen();

protected:
    bool eventFilter(QObject *obj, QEvent *event);


private:
    TouchScene *touchScene;
    QGraphicsView *view;

    QTimer *timerRR;    //用于计算报点率的Timer

    bool fullScreenFlag;

    QPushButton *clearBtn;
    QPushButton *exitBtn;
    QHBoxLayout *pHLayout;

    void createActions();
    QAction *exitAction;
    QAction *fullAction;
    QAction *cleanAction;
    QAction *showPointAction;
    QAction *showLineAction;
    QAction *showTextAction;
    QAction *switchBgAction;
    QAction *increaseLineSizeAction;
    QAction *decreaseLineSizeAction;
};

#endif // MAINWINDOW_H
