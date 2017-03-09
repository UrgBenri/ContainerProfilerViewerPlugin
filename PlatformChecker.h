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

#ifndef PLATFORMCHECKER_H
#define PLATFORMCHECKER_H

#include <QVector>
#include <stdio.h>

#include "RangeSensor.h"
#include "Position.h"
#include "UrgDrawWidget.h"

#include <QPointF>
#include <QRect>
#include <QSize>
#include <QVector>


using namespace std;
using namespace qrk;

#define LONG_PATTERN    true
#define SHORT_PATTERN   false

class PlatformChecker
{
public:
    enum {
        templX = 8,
        templY = 3,
    };

    enum Direction {
        LEFT = 0,
        RIGHT,
    };
    PlatformChecker();
    QVector<QPointF> prepareData(const QVector<QVector<QVector3D> > &points);
    QVector<QPointF> boxelCheckData(const PluginDataStructure &ranges
                                    , QRect rect
                                    , QSize size
                                    , long &xval
                                    , long &yval
                                    , int templateNumber
                                    , UrgDrawWidget* viewer = NULL);

    void plotSquare(UrgDrawWidget* viewer,
                    int plotX, int plotY,
                    int plotWidth, int plotHeight,
                    QColor color);
    void plotTemplate(UrgDrawWidget* viewer, int templ[templY][templX],
                      int plotX, int plotY, int precisionX, int precisionY,
                      QColor color);
    void resetDetection();
    bool detect(const PluginDataStructure &ranges,
                Direction dir, UrgDrawWidget* viewer = NULL);
    void setGrid(int minX_, int maxX_, int minY_, int maxY_, int precisionX_, int precisionY_);
    void setMaxDistance(int maxDist_);

    void setConfigFilename(const QString &value);
    void setSaveFilename(const QString &value);
    void setAreaNumber(int value);
private:
    int mapOffset;
    int precisionX;
    int precisionY;
    int minXValue;
    int maxXValue;
    int minYValue;
    int maxYValue;
    int maxDist;
    bool detectionState;
    QVector<int> detectionHistory;
    int historySize;
    double detectionRate;
    long lastXPos;
    long lastYPos;

    long posXTolerance;
    long posYTolerance;
};

#endif // PLATFORMCHECKER_H

