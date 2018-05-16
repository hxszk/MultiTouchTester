#ifndef TOUCHSCENE_H
#define TOUCHSCENE_H

#include <QGraphicsScene>
#include <QTouchEvent>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

struct TOUCH_POINT
{
    int id;
    int state;
    QPointF prePoint;
    QPointF curPoint;
    int reportRate;
    int reportCnt;
    TOUCH_POINT()
    {
        id = 0;
        state = 0;
        reportCnt = 0;
        reportRate = 0;
    }
};

#define MAX_TOUCH_POINT 100

class TouchScene : public QGraphicsScene
{
    Q_OBJECT

public:
    TouchScene(int sceneWidth, int sceneHeight, QObject *parent = Q_NULLPTR);

public slots:
    void updateTouchData(QList<QTouchEvent::TouchPoint> &touchPnts);
    void updateReportRate();
    void setShowPnt();
    void setShowLine();
    void setShowText();
    void cleanPanel();
    void increaseLineSize();
    void decreaseLineSize();
    void switchBG();

private:
    void initTouchScene();
    void drawTouch();

    int panelWidth;
    int panelHeight;

    int touchCnt;

    bool _isShowPnt;
    bool _isShowLine;
    bool _isShowText;

    int _lineSize;

    const int COLOR_CNT = 12;
    const QColor lineColors[12] = {Qt::red, Qt::blue, Qt::green, Qt::cyan, Qt::magenta, Qt::yellow,
                                         Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
    QPixmap* gridBG;
    QPixmap* asteriskBG;
    uchar bgType;    //0-grid, 1-asterisk;

    QList<QGraphicsEllipseItem*> ellipseList;
    QList<QGraphicsLineItem*> lineList;
    QGraphicsTextItem *textTouchInfo;

    TOUCH_POINT touchPoints[MAX_TOUCH_POINT];
};

#endif // TOUCHSCENE_H
