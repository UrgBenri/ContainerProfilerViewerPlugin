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

#include "GenericTableModel.h"

GenericTableModel::GenericTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

int GenericTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_rows.size();
}

int GenericTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_header.size();
}

QVariant GenericTableModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && (index.row() >= 0)
            && (index.row() < m_rows.size())
            && (index.column() >= 0)
            && (index.column() < m_header.size())
            && role == Qt::DisplayRole) {
        return m_rows[index.row()][index.column()];
    }

    return QVariant();
}

QVariant GenericTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole: {
            switch (orientation) {
                case Qt::Horizontal: {
                    if(section < m_header.size()) {
                        return m_header[section];
                    }
                }
                    break;
                case Qt::Vertical:  return section +1;
                default:
                    break;
            }
        }
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignHCenter;
            break;
        default:
            break;
    }
    return QVariant();
}

Qt::ItemFlags GenericTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled;
}

void GenericTableModel::clear()
{
    beginResetModel();
    m_rows.clear();
    endResetModel();
}

void GenericTableModel::setColumns(const QStringList &header)
{
    beginResetModel();
    m_header = header;
    m_rows.clear();
    endResetModel();
}

void GenericTableModel::addRow(const QStringList &data)
{
    if(data.size() == m_header.size()){
        beginInsertRows(QModelIndex(), m_rows.size(), m_rows.size());
        m_rows << data;
        endInsertRows();
    }
}

void GenericTableModel::updateRow(int position, const QStringList &data)
{
    if((position >= 0 )
            && (position < m_rows.size())
            && (data.size() == m_header.size()))
    {
        m_rows.replace(position, data);
        emit dataChanged(index(position, 0), index(position, data.size()));
    }
}

