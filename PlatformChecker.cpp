/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#include "PlatformChecker.h"
#include <cmath>
#include <limits>
#include <QDebug>
#include "MathUtils.h"
#include <ctime>
#include "delay.h"
#include "log_printf.h"

#define MAX_GRID_SIZE 100

using namespace std;

PlatformChecker::PlatformChecker()
{
    detectionState = LONG_PATTERN;

    minXValue = 100;
    maxXValue = 2000;
    minYValue = -100;
    maxYValue = 800;
    precisionX = 100;
    precisionY = 100;
    maxDist = 600;

    posXTolerance = 2;
    posYTolerance = 2;

    detectionRate = 0.5;
    historySize = 10;

    resetDetection();
}

void PlatformChecker::resetDetection()
{
    detectionState = LONG_PATTERN;

    detectionHistory.clear();

    lastXPos = 0;
    lastYPos = 0;

    mapOffset = 0;
}

QVector<QPointF> PlatformChecker::prepareData(const QVector<QVector<QVector3D>> &points)
{
    QVector<QPointF> result;
    foreach (QVector<QVector3D> vector, points) {
        if(vector.size() > 0) result << QPointF(vector[0].x(), vector[0].y());
    }
    return result;
}

void PlatformChecker::plotSquare(UrgDrawWidget* viewer,
                                 int plotX, int plotY,
                                 int plotWidth, int plotHeight,
                                 QColor color)
{
    if (viewer) {
        if (viewer->getRotationOffset() == 90) {
            viewer->addSquare(QRect((-plotX) - plotWidth, (-plotY) - plotHeight, plotWidth, plotHeight),
                              color);
        }

        if (viewer->getRotationOffset() == 270) {

            viewer->addSquare(QRect(plotX, plotY, plotWidth, plotHeight),
                              color);
        }
    }
}

void PlatformChecker::plotTemplate(UrgDrawWidget* viewer,
                                   int templ[templY][templX],
                                   int plotX, int plotY,
                                   int precisionX, int precisionY,
                                   QColor color)
{
    if (viewer) {
        for (int i = 0; i < templY; ++i) {
            for (int j = 0; j < templX; ++j) {
                if (templ[i][j] == 1) {
                    plotSquare(viewer,
                               plotX + (j * precisionX),
                               plotY + (i * precisionY),
                               precisionX, precisionY,
                               color);
                }
            }
        }
    }
}

bool PlatformChecker::detect(const PluginDataStructure &ranges,
                             Direction dir, UrgDrawWidget* viewer)
{
    long xval, yval;
    int templateNumber = 0;
    int templateState = (detectionState == LONG_PATTERN) ? 0 : 1;

    int minY = minYValue;
    int maxY = maxYValue;
    int minX = minXValue;
    int maxX = maxXValue;

    bool detected = false;

    /*
     *Check rotation to know which template set to use
     */
    if (dir == LEFT) {
        templateNumber = 0;
    }

    if (dir == RIGHT) {
        templateNumber = 2;
        swap(minY, maxY);
        minY = -(minY);
        maxY = -(maxY);
    }

    int offsetMinYValue = minY;
    int offsetMaxYValue = maxY;
    int offsetMinXValue = minX;
    int offsetMaxXValue = maxX;

    if (detectionState == LONG_PATTERN) {
        offsetMinYValue -= mapOffset;
        offsetMaxYValue -= mapOffset;
    }
    else {
        offsetMinXValue -= mapOffset;
        offsetMaxXValue -= mapOffset;
    }

    if (viewer) {
        viewer->clearSquares();
    }
    /*
     *Apply detection
     */
    QVector<QPointF> points = boxelCheckData(ranges,
                              QRect(offsetMinXValue, offsetMinYValue, offsetMaxXValue - offsetMinXValue, offsetMaxYValue - offsetMinYValue),
                              QSize(precisionX, precisionY),
                              xval, yval, templateNumber + templateState,
                              viewer);
    while (detectionHistory.size() > historySize) {
        detectionHistory.pop_front();
    }

    detected = points.size() > 0 && xval < maxDist;
    if (!detected) {
        mapOffset = mapOffset > 0 ? 0 : qMax(precisionX, precisionY) / 2;
    }

    detectionHistory.push_back(detected ? 1 : 0);

    int totalSucess = 0;
    for (int i = 0; i < detectionHistory.size(); ++i) {
        totalSucess += detectionHistory[i];
    }

    double sucessRate = (double)totalSucess / (double)historySize;
    bool detectionSucess = sucessRate > detectionRate;

    if (detectionSucess && detectionState == LONG_PATTERN) {
        detectionState = SHORT_PATTERN;
        lastXPos = xval;
        lastYPos = yval;
    }

    if (!detectionSucess && detectionState == SHORT_PATTERN) {
        detectionState = LONG_PATTERN;
        detectionHistory.clear();
    }

    if ((qAbs(xval) > (qAbs(lastXPos) + (posXTolerance * precisionX)) ||
            qAbs(yval) > (qAbs(lastYPos) + (posYTolerance * precisionY))) &&
            detectionState == SHORT_PATTERN) {
        detectionState = LONG_PATTERN;
        //            detectionSucess = false;
        detectionHistory.clear();
    }

    if (viewer && dir == LEFT) {
        viewer->addGrid(QRect(
                            QPoint((-offsetMinXValue) - (offsetMaxXValue - offsetMinXValue) , (-offsetMinYValue) - (offsetMaxYValue - offsetMinYValue)),
                            QSize(offsetMaxXValue - offsetMinXValue, offsetMaxYValue - offsetMinYValue)
                        ),
                        QSize(precisionX, precisionY), 0);
    }

    if (viewer && dir == RIGHT) {
        viewer->addGrid(QRect(
                            QPoint(offsetMinXValue , offsetMinYValue),
                            QSize(offsetMaxXValue - offsetMinXValue, offsetMaxYValue - offsetMinYValue)
                        ),
                        QSize(precisionX, precisionY), 0);
    }


    return detectionSucess;
}

QVector<QPointF> PlatformChecker::boxelCheckData(const PluginDataStructure &ranges
                                                 , QRect rect, QSize size
                                                 , long &xval, long &yval
                                                 , int templateNumber
                                                 , UrgDrawWidget* viewer)
{

    //    clock_t tStart = clock();


    if (viewer) {
        viewer->clearSquares();
    }

    // Parameter initialization
    int minXValue = rect.x();
    int maxXValue = rect.width() + rect.x();
    int minYValue = rect.y();
    int maxYValue = rect.height() + rect.y();
    int precisionX = size.width();
    int precisionY = size.height();
    int mapXSize = rect.width() / precisionX;
    int mapYSize = rect.height() / precisionY;

    int minOccupacy = 1; // 2

    // Template initalization

    int template1[templY][templX] = {
        { 2, 0, 0, 0, 0, 0, 0, 0 },
        { 2, 1, 1, 1, 1, 1, 1, 1 },
        { 2, 1, 0, 0, 0, 0, 0, 0 }
    };

    int template2[templY][templX] = {
        { 2, 2, 2, 2, 2, 2, 2, 2 },
        { 2, 1, 2, 2, 2, 2, 2, 2 },
        { 2, 1, 2, 2, 2, 2, 2, 2 }
    };

    int template3[templY][templX] = {
        { 2, 1, 0, 0, 0, 0, 0, 0 },
        { 2, 1, 1, 1, 1, 1, 1, 1 },
        { 2, 0, 0, 0, 0, 0, 0, 0 }
    };

    int template4[templY][templX] = {
        { 2, 1, 2, 2, 2, 2, 2, 2 },
        { 2, 1, 2, 2, 2, 2, 2, 2 },
        { 2, 2, 2, 2, 2, 2, 2, 2 }
    };

    int templ[templY][templX];
    memset(templ, 0, sizeof(templ));

    switch (templateNumber) {
    case 0:
        //swap(template1, templ);
        memcpy(templ, template1, sizeof(int) * (templY * templX));
        break;
    case 1:
        //        swap(template2, templ);
        memcpy(templ, template2, sizeof(int) * (templY * templX));
        break;
    case 2:
        //        swap(template3, templ);
        memcpy(templ, template3, sizeof(int) * (templY * templX));
        break;
    case 3:
        //        swap(template4, templ);
        memcpy(templ, template4, sizeof(int) * (templY * templX));
        break;
    default:
        //        swap(template1, templ);
        memcpy(templ, template1, sizeof(int) * (templY * templX));
        break;
    }

    // Data conversion from polar to cartesian coordinate system
    QVector<QVector<QVector3D>> rawPoints = ranges.converter.data()->getPoints(ranges.ranges);
    QVector<QPointF> points = prepareData(rawPoints);

    // Map initialization
    int map[MAX_GRID_SIZE][MAX_GRID_SIZE];
    memset(map, 0, sizeof(map));

    // Map population
    for (int i = 0; i < points.size(); ++i) {
        if (points[i].x() > minXValue && points[i].x() < maxXValue &&
                points[i].y() > minYValue && points[i].y() < maxYValue) {
            int x = (points[i].x() - minXValue) / precisionX;
            int y = (points[i].y() - minYValue) / precisionY;
            map[y][x]++;
        }

    }

    if (viewer) {
        for (int i = 0; i < mapYSize; ++i) {
            for (int j = 0; j < mapXSize; ++j) {
                if (map[i][j] >= minOccupacy) {
                    plotSquare(viewer,
                               (j * precisionX) + minXValue,
                               (i * precisionY) + minYValue,
                               precisionX, precisionY,
                               QColor(255, 0, 0, 25));
                }
            }
        }
    }

    plotTemplate(viewer,
                 templ,
                 maxXValue + precisionX,
                 minYValue + (rect.height() / 2),
                 precisionX, precisionY,
                 QColor(255, 0, 255, 128));

    // Result map initialization
    bool result[MAX_GRID_SIZE][MAX_GRID_SIZE];
    memset(result, false, sizeof(result));

    // Applying map and template matching in Result map
    bool detected = false;
    for (int i = 0; i < mapYSize - templY + 1; ++i) {
        for (int j = 0; j < mapXSize - templX + 1; ++j) {
            detected = true;
            for (int k = 0; k < templY; k++) {
                for (int l = 0; l < templX; l++) {
                    int val = map[i + k][j + l] >= minOccupacy ? 1 : 0;
                    if (val != templ[k][l] && templ[k][l] != 2) {
                        detected = false;
                        break;
                    }
                }
                if (!detected) {
                    break;
                }
            }
            result[i][j] = detected;
        }
    }

    if (viewer) {
        for (int i = 0; i < mapYSize; ++i) {
            for (int j = 0; j < mapXSize; ++j) {
                if (result[i][j]) {
                    plotTemplate(viewer,
                                 templ,
                                 (j * precisionX) + minXValue,
                                 (i * precisionY) + minYValue,
                                 precisionX, precisionY,
                                 QColor(0, 0, 255, 50));
                }
            }
        }
    }

    QVector<QPointF> retPoints;
    xval = numeric_limits<long>::max();
    yval = numeric_limits<long>::max();
    for (int i = 0; i < mapYSize; ++i) {
        for (int j = 0; j < mapXSize; ++j) {
            if (result[i][j]) {
                QPointF p;
                p.setX((j * precisionX) + minXValue);
                p.setY((i * precisionY) + minYValue);
                if (qAbs(xval) > qAbs(p.x())) {
                    xval = p.x();
                    yval = p.y();
                }
                if (xval == p.x() && (qAbs(yval) > qAbs(p.y()))) {
                    yval = p.y();
                }
                retPoints.push_back(p);

            }
        }
    }

    if ((xval < maxXValue) && (yval < maxYValue)) {
        plotTemplate(viewer,
                     templ,
                     xval,
                     yval,
                     precisionX,
                     precisionY,
                     QColor(0, 0, 255, 128));
    }

    //    cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;

    return retPoints;
}

void PlatformChecker::setGrid(int minX_, int maxX_, int minY_, int maxY_, int precisionX_, int precisionY_)
{
    minXValue = minX_;
    maxXValue = maxX_;
    minYValue = minY_;
    maxYValue = maxY_;
    precisionX = precisionX_;
    precisionY = precisionY_;
}

void PlatformChecker::setMaxDistance(int maxDist_)
{
    maxDist = maxDist_;
}

void PlatformChecker::setConfigFilename(const QString &value)
{

}

void PlatformChecker::setSaveFilename(const QString &value)
{

}

void PlatformChecker::setAreaNumber(int value)
{

}

