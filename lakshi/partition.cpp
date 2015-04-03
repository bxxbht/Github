#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QTimer>
#include <QCryptographicHash>
#include <QDebug>

#include "partition.h"
#include "ui_partition.h"
#include "settings.h"
#include "crc-ccitt.h"

Partition::Partition(const QString &group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Partition),
    group(group),
    settings(Settings::instance())
{
    ui->setupUi(this);
    ui->checkBox->setText(group);

    settings->beginGroup(group);
    bool enable = settings->value("enable", false).toBool();
    QString filename = settings->value("filename", "").toString();
    settings->endGroup();
    ui->checkBox->setChecked(enable);
    ui->pushButton->setEnabled(enable);
    ui->lineEdit->setText(filename);
    ui->lineEdit->setEnabled(enable);
    ui->versionLineEdit->setText(version());
    ui->versionLineEdit->setEnabled(enable);

    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(setValid(bool)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(browse()));

    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(fileChanged()));
}

Partition::~Partition()
{
    delete ui;
}

bool Partition::valid() const
{
    return ui->checkBox->checkState() == Qt::Checked;
}

bool Partition::check()
{
    const QString &iniFilename = ui->lineEdit->text();
    if (iniFilename.isEmpty()) {
        error = QString("No input file specified in %1 partition").arg(group);
        return false;
    }
    QFileInfo fileInfo = QFileInfo(iniFilename);
    if (!fileInfo.exists()) {
        error = QString("Specified file does not exist in %1 partition").arg(group);
        return false;
    }
    QFileInfo updateFileInfo = QFileInfo(fullFilename());
    if (!updateFileInfo.exists()) {
        error = QString("Update file does not exist in %1 partition").arg(group);
        return false;
    }
    const QString &sum = md5sum();
    if (!sum.isEmpty()) {
        QFile file(fullFilename());
        QCryptographicHash md5(QCryptographicHash::Md5);
        file.open(QFile::ReadOnly);
        md5.addData(file.readAll());
        if (sum != md5.result().toHex()) {
            error = QString("Md5sum does not match in %1 partition").arg(group);
            return false;
        }
    }
    ui->versionLineEdit->setText(version());
    return true;
}

QString Partition::getError() const
{
    return error;
}

bool Partition::copyToDir(const QString &dir)
{
    QFileInfo fileInfo(fullFilename());
    const QString &newFileName = dir + "/" + fileInfo.fileName();
    QFile::remove(newFileName);
    return QFile::copy(fullFilename(), newFileName);
}

QString Partition::fullFilename() const
{
    const QString &iniFilename = ui->lineEdit->text();
    QFileInfo iniFileInfo(iniFilename);
    QSettings settings(iniFilename, QSettings::IniFormat);
    return iniFileInfo.path() + '/' + settings.value("filename").toString();
}

QString Partition::filename() const
{
    const QString &iniFilename = ui->lineEdit->text();
    QFileInfo iniFileInfo(iniFilename);
    QSettings settings(iniFilename, QSettings::IniFormat);
    return QFileInfo(settings.value("filename").toString()).fileName();
}

QString Partition::version() const
{
    const QString &iniFilename = ui->lineEdit->text();
    QSettings settings(iniFilename, QSettings::IniFormat);
    return settings.value("version").toString();
}

QString Partition::md5sum() const
{
    const QString &iniFilename = ui->lineEdit->text();
    QSettings settings(iniFilename, QSettings::IniFormat);
    return settings.value("md5sum", "").toString();
}

quint16 Partition::crc() const
{
    QFile file(fullFilename());
    file.open(QFile::ReadOnly);
    int size = file.size();
    uchar *buf = file.map(0, size);
    if (size > 256 * 1024) {
        buf = &buf[size - 256 * 1024];
        size = 256 * 1024;
    }
    return crc_ccitt(0, buf, size);
}

void Partition::fileChanged()
{
    ui->versionLineEdit->setText(version());
}

void Partition::setValid(bool valid)
{
    settings->beginGroup(group);
    settings->setValue("enable", valid);
    settings->endGroup();
}

void Partition::browse()
{
    QFileInfo fileInfo(ui->lineEdit->text());
    const QString &filename = QFileDialog::getOpenFileName(this, "", fileInfo.filePath(), "Ini Format (*.ini)");
    if (filename.size() != 0) {
        settings->beginGroup(group);
        settings->setValue("filename", filename);
        settings->endGroup();
        ui->lineEdit->setText(filename);
        ui->versionLineEdit->setText(version());
    }
}
