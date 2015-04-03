#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QDebug>

#include "dialog.h"
#include "ui_dialog.h"
#include "settings.h"
#include "partition.h"


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    settings(Settings::instance())
{
    ui->setupUi(this);
    QRegExp regExp("[a-zA-Z0-9\.\-\ ]+");
    QRegExpValidator *validator = new QRegExpValidator(regExp, parent);
    ui->versionComboBox->setValidator(validator);

    const QString &version = settings->value("version", "").toString();
    const QStringList &versions = settings->value("versions", QStringList()).toStringList();
    const QString &description = settings->value("description", "").toString();
    const QString &platform = settings->value("platform", "").toString();

    if (versions.count()) {
        ui->versionComboBox->addItems(versions);
    }
    ui->versionComboBox->setEditText(version);
    ui->descriptionLineEdit->setText(description);

    ui->platformComboBox->addItem("Venus", "vitas-2.0");
    ui->platformComboBox->addItem("OLiveIII", "olive-3.0");
    ui->platformComboBox->setCurrentIndex(ui->platformComboBox->findData(platform));

    settings->beginGroup("Output");
    const QString &outputFormat = settings->value("format", "usb-disk").toString();
    const QString &outputDir = settings->value("directory", "").toString();
    const QStringList &outputDirs = settings->value("outputDirs", QStringList()).toStringList();
    settings->endGroup();

    if (outputFormat == "usb-disk") {
        ui->selfExtractingFormatRadioButton->setChecked(false);
        ui->usbDiskRadioButton->setChecked(true);
    } else {
        ui->selfExtractingFormatRadioButton->setChecked(true);
        ui->usbDiskRadioButton->setChecked(false);
    }

    if (outputDirs.count()) {
        ui->outputDirectoryComboBox->addItems(outputDirs);
    }
    ui->outputDirectoryComboBox->setEditText(outputDir);

    connect(ui->versionComboBox, SIGNAL(activated(QString)), this, SLOT(versionEdited(QString)));
    connect(ui->removeVersionPushButton, SIGNAL(clicked()), this, SLOT(removeVersionItem()));
    connect(ui->descriptionLineEdit, SIGNAL(textEdited(QString)), this, SLOT(descriptionEdited(QString)));
    connect(ui->platformComboBox, SIGNAL(activated(int)), this, SLOT(platformEdited(int)));
    connect(ui->usbDiskRadioButton, SIGNAL(clicked()), this, SLOT(formatChanged()));
    connect(ui->selfExtractingFormatRadioButton, SIGNAL(clicked()), this, SLOT(formatChanged()));
    connect(ui->browsePushButton, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->removeOutputDirPushButton, SIGNAL(clicked()), this, SLOT(removeOutputDirItem()));

    QLayout *sectionLayout = ui->sectionGroupBox->layout();

    bootPartition = new Partition("Boot", this);
    sectionLayout->addWidget(bootPartition);
    groups.append(bootPartition);

    logoPartition = new Partition("Logo", this);
    sectionLayout->addWidget(logoPartition);
    groups.append(logoPartition);

    linuxPartition = new Partition("Linux", this);
    sectionLayout->addWidget(linuxPartition);
    groups.append(linuxPartition);

    rootfsPartition = new Partition("Rootfs", this);
    sectionLayout->addWidget(rootfsPartition);
    groups.append(rootfsPartition);

    appPartition = new Partition("App", this);
    sectionLayout->addWidget(appPartition);
    groups.append(appPartition);

    modulePartition = new Partition("Module", this);
    sectionLayout->addWidget(modulePartition);
    groups.append(modulePartition);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::versionEdited(const QString &text)
{
    settings->setValue("version", text);
}

void Dialog::removeVersionItem()
{
    int index = ui->versionComboBox->currentIndex();
    if (index != -1) {
        ui->versionComboBox->removeItem(index);
        QStringList versions;
        int count = ui->versionComboBox->count();
        for (int i = 0; i < count; i++) {
            versions << ui->versionComboBox->itemText(i);
        }
        settings->setValue("versions", versions);
    }
}

void Dialog::descriptionEdited(const QString &text)
{
    settings->setValue("description", text);
}

void Dialog::platformEdited(int index)
{
    settings->setValue("platform", ui->platformComboBox->itemData(index));
}

void Dialog::formatChanged()
{
    settings->beginGroup("Output");
    if (ui->usbDiskRadioButton->isChecked()) {
        settings->setValue("format", "usb-disk");
    } else {
        settings->setValue("format", "self-extracting");
    }
    settings->endGroup();
}

void Dialog::browse()
{
    const QString &directory = QFileDialog::getExistingDirectory(this, "", ui->outputDirectoryComboBox->currentText());
    if (directory.size() != 0) {
        settings->beginGroup("Output");
        settings->setValue("directory", directory);
        if (ui->outputDirectoryComboBox->findText(directory) == -1) {
            ui->outputDirectoryComboBox->addItem(directory);
            QStringList outputs;
            int count = ui->outputDirectoryComboBox->count();
            for (int i = 0; i < count; i++) {
                outputs << ui->outputDirectoryComboBox->itemText(i);
            }
            settings->setValue("outputDirs", outputs);
        }
        settings->endGroup();
        ui->outputDirectoryComboBox->setEditText(directory);

    }
}

void Dialog::removeOutputDirItem()
{
    int index = ui->outputDirectoryComboBox->currentIndex();
    if (index != -1) {
        ui->outputDirectoryComboBox->removeItem(index);
        QStringList outputs;
        int count = ui->outputDirectoryComboBox->count();
        for (int i = 0; i < count; i++) {
            outputs << ui->outputDirectoryComboBox->itemText(i);
        }
        settings->beginGroup("Output");
        settings->setValue("outputDirs", outputs);
        settings->endGroup();
    }
}

void Dialog::save()
{
    QStringList errorList;
    int validPartition = 0;
    QListIterator<Partition *> partitions(groups);
    while (partitions.hasNext()) {
        Partition *partition = partitions.next();
        if (partition->valid()) {
            if (partition->check())
                validPartition++;
            else
                errorList << partition->getError();
        }
    }

    if (errorList.count()) {
        QMessageBox::critical(this, "Update Package Manager", errorList.join("\n"));
    } else if (validPartition > 0) {
        createPackage();
    } else {
        QMessageBox::information(this, "Update Package Manager", "You must at least select one partition");
    }
}

void Dialog::createPackage()
{
    const QString &version = ui->versionComboBox->currentText();
    if (ui->versionComboBox->findText(version) == -1) {
        ui->versionComboBox->addItem(version);
        QStringList versions;
        int count = ui->versionComboBox->count();
        for (int i = 0; i < count; i++) {
            versions << ui->versionComboBox->itemText(i);
        }
        settings->setValue("versions", versions);
    }
    const QString &description = ui->descriptionLineEdit->text();
    if (version.isEmpty()) {
        QMessageBox::critical(this, "", "Invalid version");
        return;
    }
    QString outputDir = ui->outputDirectoryComboBox->currentText();
    QFileInfo outputDirInfo(outputDir);
    if (!outputDirInfo.exists()) {
        QMessageBox::critical(this, "", "Output directory does not exist");
        return;
    }
    if (!outputDirInfo.isWritable() || !outputDirInfo.isDir()) {
        QMessageBox::critical(this, "", "Output directory is readonly");
        return;
    }

    if (outputDirInfo.fileName() != "usbupdate")
        outputDir.append("/usbupdate");
    outputDirInfo.setFile(outputDir + "/" + version);
    if (outputDirInfo.exists()) {
        if (QMessageBox::warning(this, "",
                QString("%1 was already exist\nDo you want to replace it?").arg(version),
                QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }

    outputDir = outputDirInfo.absoluteFilePath();
    QSettings install(outputDir + "/install.ini", QSettings::IniFormat);
    install.clear();
    install.setValue("magic", "maya");
    install.setValue("version", version);
    install.setValue("description", description);
    int currentIndex = ui->platformComboBox->currentIndex();
    if (currentIndex != -1)
        install.setValue("platform", ui->platformComboBox->itemData(currentIndex));

    QListIterator<Partition *> partitions(groups);
    while (partitions.hasNext()) {
        Partition *partition = partitions.next();
        if (partition->valid()) {
            install.beginGroup(partition->getGroup());
            install.setValue("filename", partition->filename());
            install.setValue("version", partition->version());
            install.setValue("validate", true);
            install.setValue("crc", partition->crc());
            install.endGroup();
            install.sync();
            partition->copyToDir(outputDir);
        }
    }

    QMessageBox::information(this, "", "Build completed.\n\n"
                                       "Don't forget to umount USB disk\n"
                                       "before you disconect the device");
}
