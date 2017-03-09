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

#ifndef FILESELECTORWIDGET_H
#define FILESELECTORWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QEvent>

class QLineEditEx;

class FileSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSelectorWidget(QWidget *parent = 0);

    enum SelectionMode{
        OpenMode = 0,
        SaveMode,
    };
    Q_ENUM(SelectionMode)

    QString filename() const;
    inline QString filenameFilter() const { return m_filenameFilter; }
    inline SelectionMode mode() const { return m_mode; }

protected:
    void changeEvent(QEvent *e);

signals:
    void filenameChanged(const QString &filename);

public slots:
    inline void setMode(const SelectionMode &mode) { m_mode = mode; }
    inline void setFilenameFilter(const QString &filenameFilter) { m_filenameFilter = filenameFilter; }
    void clear();
    void setFilename(const QString &name);

private slots:
    void browseFile();

private:
    QLineEditEx *m_filename;
    QPushButton *m_browse;
    QString m_filenameFilter;
    SelectionMode m_mode;
};

#endif // FILESELECTORWIDGET_H

