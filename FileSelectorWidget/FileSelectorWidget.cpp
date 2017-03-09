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

#include "FileSelectorWidget.h"

#include <QHBoxLayout>
#include <QFileDialog>

#include "QLineEditEx.h"

FileSelectorWidget::FileSelectorWidget(QWidget *parent)
    : QWidget(parent)
    , m_filename(new QLineEditEx(this))
    , m_browse(new QPushButton(this))
    , m_filenameFilter("CSV file (*.csv)")
    , m_mode(OpenMode)
{
    m_browse->setText("...");
    m_browse->setMaximumWidth(32);
    m_filename->setReadOnly(true);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(m_filename);
    layout->addWidget(m_browse);

    connect(m_browse, &QPushButton::clicked,
            this, &FileSelectorWidget::browseFile);
    connect(m_filename, &QLineEditEx::clicked,
            this, &FileSelectorWidget::browseFile);
}

QString FileSelectorWidget::filename() const
{
    return m_filename->text();
}

void FileSelectorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            m_filename->setPlaceholderText(tr("Select a file"));
            break;
        default:
            break;
    }
}

void FileSelectorWidget::clear()
{
    m_filename->setText("");
}

void FileSelectorWidget::setFilename(const QString &name)
{
    if(m_filename->text() != name){
        m_filename->setText(name);
        filenameChanged(name);
    }
}

void FileSelectorWidget::browseFile()
{
    QString filename;
    switch (m_mode) {
        case OpenMode:
            filename = QFileDialog::getOpenFileName(
                        this,
                        tr("Open file"),
                        QDir::currentPath(),
                        m_filenameFilter);
            break;
        case SaveMode:
            filename = QFileDialog::getSaveFileName(
                        this,
                        tr("Save file"),
                        QDir::currentPath(),
                        m_filenameFilter);
            break;
        default:
            break;
    }
    if(m_filename->text() != filename){
        m_filename->setText(filename);
        filenameChanged(filename);
    }
}

