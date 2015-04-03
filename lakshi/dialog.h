#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class Dialog;
}

class Settings;
class Partition;

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void versionEdited(const QString &text);
    void removeVersionItem();
    void descriptionEdited(const QString &text);
    void platformEdited(int index);
    void formatChanged();
    void browse();
    void removeOutputDirItem();
    void save();

private:
    void createPackage();
    
private:
    Ui::Dialog *ui;
    Settings *settings;

    Partition *bootPartition;
    Partition *logoPartition;
    Partition *linuxPartition;
    Partition *rootfsPartition;
    Partition *appPartition;
    Partition *modulePartition;

    QList <Partition *> groups;
};

#endif // DIALOG_H
