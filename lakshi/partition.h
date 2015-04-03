#ifndef PARTITION_H
#define PARTITION_H

#include <QWidget>

namespace Ui {
class Partition;
}

class Settings;
class QFileSystemWatcher;

class Partition : public QWidget
{
    Q_OBJECT
    
public:
    explicit Partition(const QString &group, QWidget *parent = 0);
    ~Partition();

    bool valid() const;
    bool check();
    QString getError() const;
    bool copyToDir(const QString &dir);
    QString fullFilename() const;
    QString filename() const;
    QString version() const;
    QString md5sum() const;
    quint16 crc() const;
    QString getGroup() const
    {
        return group;
    }

public slots:
    void fileChanged();
    void setValid(bool valid);
    void browse();
    
private:
    Ui::Partition *ui;
    QString group;
    Settings *settings;
    QString error;
};

#endif // FILEPARTITION_H
