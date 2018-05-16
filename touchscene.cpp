#include "touchscene.h"

#include <QPainter>

TouchScene::TouchScene(int sceneWidth, int sceneHeight, QObject *parent):
    QGraphicsScene(parent)
{
    panelWidth = sceneWidth;
    panelHeight = sceneHeight;
    touchCnt = 0;

    _isShowPnt = false;
    _isShowLine = true;
    _isShowText = true;
    _lineSize = 2;
    bgType = 0;

    lineList.clear();
    ellipseList.clear();

    initTouchScene();

    textTouchInfo = this->addText(" ");
    //第一次设置隐藏会导致开始画线时卡顿，但是正常运行中随意设置都OK，暂时默认显示
    //textTouchInfo->setVisible(_isShowText);
}

void TouchScene::initTouchScene()
{
    this->setSceneRect(0, 0, panelWidth, panelHeight);

    const int gridSize = 50;
    gridBG = new QPixmap(gridSize, gridSize);
    QPainter gridPainter(gridBG);

    gridPainter.setPen(QColor(0xFFFFFF));
    gridPainter.setBrush(QColor(0xFFFFFF));
    gridPainter.drawRect(0, 0, gridSize, gridSize);

    gridPainter.setPen(QColor(0xD9D9D9));
    gridPainter.drawLine(0, gridSize-1, gridSize-1, gridSize-1);
    gridPainter.drawLine(gridSize-1, 0, gridSize-1, gridSize-1);


    /*border must large than range*/
    const int border = 15;
    const int range = 5;
    asteriskBG = new QPixmap(panelWidth, panelHeight);
    QPainter asteriskPainter(asteriskBG);
    QPen pen;

    //painter.setRenderHint(QPainter::Antialiasing, true);
    asteriskPainter.setPen(Qt::NoPen);
    asteriskPainter.setBrush(QColor(0xFFFFFF));
    asteriskPainter.drawRect(0, 0, panelWidth, panelHeight);

    pen.setColor(QColor(0xA0A0A0));
    pen.setStyle(Qt::DashLine);
    asteriskPainter.setPen(pen);

    QPoint topLeft  (border, border);
    QPoint topMid   (panelWidth/2, border);
    QPoint topRight (panelWidth-1-border, border);
    QPoint midLeft  (border, panelHeight/2);
    QPoint midRight (panelWidth-1-border, panelHeight/2);
    QPoint botLeft  (border, panelHeight-1-border);
    QPoint botMid   (panelWidth/2, panelHeight-1-border);
    QPoint botRight (panelWidth-1-border, panelHeight-1-border);

    asteriskPainter.drawLine(topLeft, topRight);
    asteriskPainter.drawLine(midLeft, midRight);
    asteriskPainter.drawLine(botLeft, botRight);
    asteriskPainter.drawLine(topLeft, botLeft);
    asteriskPainter.drawLine(topMid, botMid);
    asteriskPainter.drawLine(topRight, botRight);
    asteriskPainter.drawLine(topLeft, botRight);
    asteriskPainter.drawLine(topRight, botLeft);

    pen.setStyle(Qt::SolidLine);
    asteriskPainter.setPen(pen);
    asteriskPainter.drawLine(topLeft+QPoint(-range,-range), topRight+QPoint(range,-range));
    asteriskPainter.drawLine(topRight+QPoint(range,-range), botRight+QPoint(range,range));
    asteriskPainter.drawLine(botRight+QPoint(range,range), botLeft+QPoint(-range,range));
    asteriskPainter.drawLine(botLeft+QPoint(-range,range), topLeft+QPoint(-range,-range));

    asteriskPainter.drawLine(topLeft+QPoint(range,range), topRight+QPoint(-range,range));
    asteriskPainter.drawLine(topRight+QPoint(-range,range), botRight+QPoint(-range,-range));
    asteriskPainter.drawLine(botRight+QPoint(-range,-range), botLeft+QPoint(range,-range));
    asteriskPainter.drawLine(botLeft+QPoint(range,-range), topLeft+QPoint(range,range));

    asteriskPainter.drawLine(topMid+QPoint(-range,-range), botMid+QPoint(-range,range));
    asteriskPainter.drawLine(topMid+QPoint(range,-range), botMid+QPoint(range,range));

    asteriskPainter.drawLine(midLeft+QPoint(-range,-range), midRight+QPoint(range,-range));
    asteriskPainter.drawLine(midLeft+QPoint(-range,range), midRight+QPoint(range,range));

    const int diagonalRange_x = range * 204 / 100;
    const int diagonalRange_y = range * 115 / 100; //
    asteriskPainter.drawLine(topLeft+QPoint(diagonalRange_x,0), botRight+QPoint(0,-diagonalRange_y));
    asteriskPainter.drawLine(topLeft+QPoint(0,diagonalRange_y), botRight+QPoint(-diagonalRange_x,0));

    asteriskPainter.drawLine(topRight+QPoint(-diagonalRange_x,0), botLeft+QPoint(0,-diagonalRange_y));
    asteriskPainter.drawLine(topRight+QPoint(0,diagonalRange_y), botLeft+QPoint(diagonalRange_x,0));

    if(bgType == 0)
    {
        this->setBackgroundBrush(QBrush(*gridBG));
    }
    else
    {
        this->setBackgroundBrush(QBrush(*asteriskBG));
    }
}

void TouchScene::updateReportRate()
{
    int i = 0, j = 0;
    for(j = 0; j < MAX_TOUCH_POINT; j++)
    {
        if(touchPoints[j].state & ( Qt::TouchPointPressed
                                   |Qt::TouchPointMoved
                                   |Qt::TouchPointReleased))
        {
            touchPoints[j].reportRate = touchPoints[j].reportCnt;
            touchPoints[j].reportCnt = 0;
            i++;
        }

        if( i == touchCnt)
        {
            break;
        }
    }
}

void TouchScene::updateTouchData(QList<QTouchEvent::TouchPoint> &touchPnts)
{
    int i = 0, j = 0;

    foreach (QTouchEvent::TouchPoint pnt, touchPnts)
    {
        switch(pnt.state())
        {
            case Qt::TouchPointPressed:
                for(j = 0; j < MAX_TOUCH_POINT; j++)
                {
                    if(touchPoints[j].state == 0)
                    {
                        touchPoints[j].state = Qt::TouchPointPressed;
                        touchPoints[j].id = pnt.id();
                        touchPoints[j].prePoint = pnt.lastPos();
                        touchPoints[j].curPoint = pnt.pos();
                        touchPoints[j].reportCnt = 1;
                        break;
                    }
                }
            break;

            case Qt::TouchPointMoved:
                for(j = 0; j < MAX_TOUCH_POINT; j++)
                {
                    if(touchPoints[j].id == pnt.id())
                    {
                        touchPoints[j].state = Qt::TouchPointMoved;
                        touchPoints[j].prePoint = pnt.lastPos();
                        touchPoints[j].curPoint = pnt.pos();
                        touchPoints[j].reportCnt += 1;
                        break;
                    }
                }
            break;

            case Qt::TouchPointStationary:
            break;

            case Qt::TouchPointReleased:
                for(j = 0; j < MAX_TOUCH_POINT; j++)
                {
                    if(touchPoints[j].id == pnt.id())
                    {
                        touchPoints[j].state = Qt::TouchPointReleased;
                        touchPoints[j].prePoint = pnt.lastPos();
                        touchPoints[j].curPoint = pnt.pos();
                        break;
                    }
                }
                break;

            default:

                break;
        }

        i++;
    }
    touchCnt = i;

    drawTouch();
}


void TouchScene::drawTouch()
{
    QGraphicsEllipseItem* ellipse = 0;
    QGraphicsLineItem* line = 0;

    QPen pen;
    QString text;
    int ellipseSize = _lineSize + 2 + _lineSize%2;
    int i = 0, j = 0;

    text.append("TouchCnt:").append(QString::number(touchCnt)).append("\r\n");

    i = 0;
    for(j = 0; j < MAX_TOUCH_POINT; j++)
    {
        if(touchPoints[j].state & ( Qt::TouchPointPressed
                                   |Qt::TouchPointMoved
                                   |Qt::TouchPointReleased))
        {
            /*set point color*/
            uchar colorIdx = (touchPoints[j].id) % COLOR_CNT;
            QColor pntColor = lineColors[colorIdx];
            pen.setColor(pntColor);

            /*add touch info*/
            text.append("id:").append(QString::number(j))
                .append(" state:").append(QString::number(touchPoints[j].state))
                .append(" (").append(QString::number((int)touchPoints[j].curPoint.x()))
                .append(",").append(QString::number((int)touchPoints[j].curPoint.y())).append(")")
                .append(" RR:").append(QString::number(touchPoints[j].reportRate))
                .append("\r\n");

            /*draw point*/
            ellipse = addEllipse(touchPoints[j].curPoint.x() - ellipseSize/2, touchPoints[j].curPoint.y()- ellipseSize/2,
                                 ellipseSize, ellipseSize,pen, QBrush(pntColor));
            ellipse->setVisible(_isShowPnt);
            ellipseList.push_back(ellipse);

            /*draw line*/
            pen.setWidth(_lineSize);
            line = addLine(touchPoints[j].prePoint.x(), touchPoints[j].prePoint.y(),
                           touchPoints[j].curPoint.x(), touchPoints[j].curPoint.y(), pen);
            line->setVisible(_isShowLine);
            lineList.push_back(line);
        }

        if(touchPoints[j].state == Qt::TouchPointReleased)
        {
            touchPoints[j].state = 0;
            touchPoints[j].id = 0;
            touchPoints[j].reportRate = 0;
            i++;
        }

        if( i == touchCnt)
        {
            break;
        }
    }
    textTouchInfo->setPlainText(text);
/*
    foreach (QTouchEvent::TouchPoint pnt, touchPnts)
    {
        //get low 16bit as point id
        uint pntID = pnt.id() & 0xFFFF;
        uchar colorIdx = (pntID-1) % COLOR_CNT;
        QColor pntColor = lineColors[colorIdx];
        pen.setColor(pntColor);

        text.append("id:").append(QString::number(pntID))
            .append(" state:").append(QString::number(pnt.state()))
            .append(" (").append(QString::number((int)pnt.pos().x()))
            .append(",").append(QString::number((int)pnt.pos().y())).append(")")
            .append("\r\n");

        switch(pnt.state())
        {
            case Qt::TouchPointPressed:
            case Qt::TouchPointMoved:
            case Qt::TouchPointReleased:
                pen.setWidth(0);

                ellipse = addEllipse(pnt.pos().x() - ellipseSize/2, pnt.pos().y()- ellipseSize/2,
                                                           ellipseSize, ellipseSize,pen, QBrush(pntColor));
                ellipse->setVisible(_isShowPnt);
                ellipseList.push_back(ellipse);

                pen.setWidth(_lineSize);
                line = addLine(pnt.lastPos().x(), pnt.lastPos().y(),
                                                  pnt.pos().x(), pnt.pos().y(), pen);
                line->setVisible(_isShowLine);
                lineList.push_back(line);
                break;

            case Qt::TouchPointStationary:
                break;

            default:
                break;
        }
    }
    */
}

void TouchScene::setShowPnt(void)
{
    this->_isShowPnt = !this->_isShowPnt;

    foreach (QGraphicsEllipseItem* ellipse, ellipseList) {
        ellipse->setVisible(_isShowPnt);
    }
}

void TouchScene::setShowLine()
{
    this->_isShowLine = !this->_isShowLine;

    foreach (QGraphicsLineItem* line, lineList) {
        line->setVisible(_isShowLine);
    }
}

void TouchScene::setShowText()
{
    this->_isShowText = !this->_isShowText;

    textTouchInfo->setVisible(_isShowText);
}

void TouchScene::cleanPanel()
{
    this->clear();
    this->lineList.clear();
    this->ellipseList.clear();

    textTouchInfo = this->addText(" ");
    textTouchInfo->setVisible(_isShowText);
}

void TouchScene::increaseLineSize()
{
    qDebug("LineSize %d", _lineSize);
    this->_lineSize = _lineSize > 5 ? _lineSize : (_lineSize+1);
}

void TouchScene::decreaseLineSize()
{
    this->_lineSize = _lineSize < 2 ? _lineSize : (_lineSize-1);
}

void TouchScene::switchBG()
{
    this->bgType = !bgType;

    if(bgType == 0)
    {
        this->setBackgroundBrush(QBrush(*gridBG));
    }
    else
    {
        this->setBackgroundBrush(QBrush(*asteriskBG));
    }
}
