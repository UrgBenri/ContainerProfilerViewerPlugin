#include "SettingsModel.h"

SettingsModel::SettingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

SettingsModel::~SettingsModel()
{

}


int SettingsModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole)
      {
       return m_data[index.row()].value;
      }
    return QVariant::Invalid;
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        m_data[index.row()].value = value;
        emit dataChanged(index, index);
        emit valueChanged(m_data[index.row()].title, m_data[index.row()].value);
        if(m_data[index.row()].func){
            m_data[index.row()].func(value);
        }
        return true;
    }
    return false;
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            return tr("Value");
        }
        else if(orientation == Qt::Vertical){
            return m_data[section].title;
        }
    }

    return QVariant::Invalid;
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

void SettingsModel::append(const QString &key
                           , const QVariant &value
                           , std::function<void(const QVariant &)> func)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    data_t data;
    data.title = key;
    data.value = value;
    data.func = func;
    m_data << data;
    endInsertRows();
}
