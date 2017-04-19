#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractTableModel>
#include <Qvector>
#include <QVariant>
#include <QString>
#include <functional>

class SettingsModel
        : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SettingsModel(QObject *parent = Q_NULLPTR);
    virtual ~SettingsModel();

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    void append(const QString &key
                , const QVariant &value
                , std::function<void(const QVariant &)> func = Q_NULLPTR);

signals:
    void valueChanged(const QString &key, const QVariant &value);

private:
    typedef struct {
        QString title;
        QVariant value;
        std::function<void(const QVariant &)> func;
    } data_t;
    QVector<data_t> m_data;
};

#endif // SETTINGSMODEL_H
