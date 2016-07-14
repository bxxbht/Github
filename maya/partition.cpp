/*
 * Maya - software update utility
 *
 * Copyright (c) 2015, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QTimer>
#include <QSettings>
#include <QProcess>
#include <QDebug>

#include "hal.h"
#include "partition.h"

#define SZ_1K   1024
#define SZ_1M   1024 * 1024

QMap<QString, Partition::PartInfo *> Partition::partInfoMap;

QList<PartitionPackage *> Partition::partitionPackages;

Partition::Partition(const QString &group, QObject *parent) :
    QThread(parent),
    group(group),
    partInfo(partInfoMap[group]),
    status(Success),
    progressValue(0)
{
}

Partition::~Partition()
{
}

void Partition::mapGroup()
{
    if (partInfoMap.empty()) {
        partInfoMap["Boot"] = new PartInfo("boot", SZ_1K, 768 * SZ_1K);
        partInfoMap["Linux"] = new PartInfo("linux", SZ_1M, 4 * SZ_1M);
        partInfoMap["Rootfs"] = new PartInfo("rootfs", 64 * SZ_1M, "/");
        partInfoMap["App"] = new PartInfo("app", 16 * SZ_1M, "/appdir");

        QString machType(Hal::instance()->machine());
        if (machType.contains("olive", Qt::CaseInsensitive)) {
            partInfoMap["Printer"] = new PartInfo("printer", 256 * SZ_1K, "/dev/ttymxc2", IAP);
        } else if(machType.contains("polar", Qt::CaseInsensitive)) {
            partInfoMap["Module"] = new PartInfo("module", 256 * SZ_1K, "/dev/ttymxc2", ISP);
            partInfoMap["Fetal"] = new PartInfo("fetal", 768 * SZ_1K, "", FetalIAP);
        } else {
            /* venus */
            partInfoMap["Module"] = new PartInfo("module", 256 * SZ_1K, "/dev/ttymxc2", ISP);
            partInfoMap["IBP"] = new PartInfo("ibp", 256 * SZ_1K, "/dev/ttymxc3", ISP);
            partInfoMap["Printer"] = new PartInfo("printer", 256 * SZ_1K, "/dev/ttymxc4", IAP);
        }
    }
}

PartitionPackage *Partition::createPackage(int index, const QString &version)
{
    PartitionPackage *package = new PartitionPackage(index, version);
    partitionPackages.append(package);
    return package;
}

Partition *Partition::createPartition(const QString &group)
{
    mapGroup();
    if (!partInfoMap.contains(group))
        return 0;

    Partition *part;
#ifdef __arm__
    Type type = partInfoMap[group]->type;
#else
    Type type = Dummy;
#endif
    switch (type) {
    case Dummy:
        part = new DummyPartition(group);
        break;
    case MMC:
        part = new MmcPartition(group);
        break;
    case Ext2:
        part = new Ext2Partition(group);
        break;
    case ISP:
        part = new IspPartition(group);
        break;
    case IAP:
        part = new IapPartition(group);
        break;
    case FetalIAP:
        part = new FetalIapPartition(group);
        break;
    default:
        part = 0;
    }

    return part;
}

PartitionPackage *Partition::insertPackage(int index, const QString &directory)
{
    const QString &installIni = directory + "/install.ini";
    QSettings settings(installIni, QSettings::IniFormat);

    if (settings.status() != QSettings::NoError)
        return NULL;
    if (settings.value("magic", "").toString() != "maya")
        return NULL;
    const QString &version = settings.value("version", "").toString();
    if (version.isEmpty())
        return NULL;
    const QString platform = settings.value("platform", "").toString();
#ifdef __arm__
    const char *machType = Hal::instance()->machine();
#else
    const char *machType = "mx6dl-venus-1.0";
#endif
    if (platform.isEmpty() || !machType || (platform.compare(machType) != 0))
        return NULL;

    PartitionPackage *package = createPackage(index, version);

    QStringList groups = settings.childGroups();
    QStringListIterator groupIterator(groups);
    while (groupIterator.hasNext()) {
        const QString group = groupIterator.next();
        Partition *part = createPartition(group);
        if (!part)
            continue;

        package->insert(part);

        settings.beginGroup(group);
        QStringList childKeys = settings.childKeys();
        if (childKeys.contains("filename") && \
                childKeys.contains("version") && \
                childKeys.contains("validate") && \
                childKeys.contains("crc")) {

            const QString &filename = settings.value("filename").toString();
            const QString &fullFilename = QString("%1/%2").arg(directory, filename);
            const QString &version = settings.value("version").toString();
            bool validate = settings.value("validate").toBool();
            uint crc = settings.value("crc").toUInt();

            settings.beginGroup(group);
            const QString &oldVersion = settings.value("version", "Unknown").toString();
            settings.endGroup();

            part->setFileInfo(fullFilename, version, oldVersion, validate, crc);

            QFile file(fullFilename);
            QFileInfo fileInfo(fullFilename);
            if (!file.exists()) {
                part->setStatus(Failed);
                part->setError(FileNotExist, "<span style='color:#ff0000;'>File Not Exist</span>");
            } else if (validate) {
                qint64 size = fileInfo.size();
                file.open(QIODevice::ReadOnly);
                const char *buf = (const char *)file.map(0, size);
                if (size > 16 * 1024) {
                    buf = &buf[size - 16 * 1024];
                    size = 16 * 1024;
                }
                if (crc != qChecksum(buf, size)) {
                    part->setStatus(Failed);
                    part->setError(CrcError, "<span style='color:#ff0000;'>CRC Error</span>");
                }
            }
        } else {
            part->setStatus(Failed);
            part->setError(Incomplted);
        }
        settings.endGroup();
    }
    return package;
}

bool Partition::validatePackage(int index)
{
    PartitionPackage *pack = package(index);
    if (!pack)
        return false;

    QListIterator<Partition *> i(pack->partitions);
    while (i.hasNext()) {
        Partition *part = i.next();
        if (part->status == Partition::Failed)
            return false;
    }

    return true;
}

PartitionPackage *Partition::package(int index)
{
    PartitionPackage *pack = NULL;
    if (index >= 0) {
        QListIterator<PartitionPackage *> i(partitionPackages);
        while (i.hasNext()) {
            PartitionPackage *p = i.next();
            if (p->index == index) {
                pack = p;
                break;
            }
        }
    }
    return pack;
}

QString Partition::getOldVersion()
{
    const QString &version = getOldVersion(group);
    if (!version.isEmpty()) {
        oldVersion = version;
    }
    return oldVersion;
}

QString Partition::getOldVersion(const QString &group)
{
    QString version;
    if (group == "Boot") {
        version = Hal::instance()->bootVersion();
    } else if (group == "Linux") {
        version = Hal::instance()->kernelVersion();
    } else if (group == "Rootfs") {
        version = Hal::instance()->rootVersion();
    } else if (group == "App") {
        QSettings settings("/appdir/version.ini", QSettings::IniFormat);
        version = settings.value("version", "").toString();
    } else if (group == "Module") {

    }
    return version;
}

void Partition::run()
{
    bool ret;
#ifndef __arm__
    emit statusChanged("<span style='color:cyan;'>Probing</span>");
    msleep(500);
    emit statusChanged("<span style='color:yellow;'>Unassembled</span>");
    msleep(500);
    emit statusChanged("<span style='color:cyan;'>Updating</span>");
    msleep(500);

    while (progressValue++ < 100) {
        emit progressChanged(progressValue);
        usleep(20 * 1000);
    }

    ret = true;
    emit statusChanged("<span style='color:green;'>Success</span>");
#else
    ret = write();
#endif

    if (ret != true)
        setStatus(Failed);

    exit(ret != true);
}

DummyPartition::DummyPartition(const QString &group, QObject *parent) :
    Partition(group, parent)
{
}

DummyPartition::~DummyPartition()
{
}

bool DummyPartition::write()
{
    emit statusChanged("<span style='color:cyan;'>Probing</span>");
    sleep(1);
    emit statusChanged("<span style='color:yellow;'>Unassembled</span>");
    sleep(1);
    emit statusChanged("<span style='color:cyan;'>Updating</span>");
    sleep(1);

    while (progressValue++ < 100) {
        emit progressChanged(progressValue);
        usleep(20 * 1000);
    }
    emit statusChanged("<span style='color:green;'>Success</span>");
    return true;
}


MmcPartition::MmcPartition(const QString &group, QObject *parent) :
    Partition(group, parent)
{
}

MmcPartition::~MmcPartition()
{
}

bool MmcPartition::write()
{
    emit statusChanged("<span style='color:cyan;'>Updating</span>");

    qDebug() << "MMC write:" << partInfo->devn << ", offset:" << partInfo->offset << ", at" << group;
    QFile mmc(partInfo->devn);
    if (!mmc.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
        emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(mmc.errorString()));
        return false;
    }
    mmc.seek(partInfo->offset);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(file.errorString()));
        return false;
    }

    int size = file.size();
    int progress = 0;

    if (group == "Boot") {
        size -= 1024;
        file.seek(1024);
        destoryenv();
    }

    progressValue = 0;
    emit progressChanged(progressValue);

    QByteArray data;
    while ((data = file.read(512)).size() > 0) {
        if (mmc.write(data) < 0) {
            emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(mmc.errorString()));
            return false;
        }
        progress += data.size();
        progressValue = 100 * progress / size;
        emit progressChanged(progressValue);
    }

    file.close();
    mmc.close();

    emit statusChanged("<span style='color:green;'>Success</span>");

    return true;
}

void MmcPartition::destoryenv()
{
    QFile mmc(partInfo->devn);
    if (!mmc.open(QIODevice::ReadWrite | QIODevice::Unbuffered)) {
        return;
    }
    mmc.seek(768 * SZ_1K);
    mmc.write(QByteArray(8 * SZ_1K, 0xFF));
    mmc.close();
}

Ext2Partition::Ext2Partition(const QString &group, QObject *parent) :
    Partition(group, parent)
{
}

Ext2Partition::~Ext2Partition()
{
}

bool Ext2Partition::write()
{
    QFileInfo fileInfo(filename);
    QString tmpGz = "/tmp/" + fileInfo.fileName();
    QString tmpExt2 = "/tmp/" + fileInfo.completeBaseName();

    emit statusChanged("<span style='color:cyan;'>Updating</span>");

    progressValue = 0;
    emit progressChanged(progressValue);
    int ret = QProcess::execute("cp", QStringList() << filename << tmpGz);
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Copy Failed</span>"));
        return false;
    }

    progressValue = 10;
    emit progressChanged(progressValue);
    QProcess::execute("rm", QStringList() << "-f" << tmpExt2);
    ret = QProcess::execute("gunzip", QStringList() << tmpGz);
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Uncompress Failed</span>"));
        return false;
    }

    progressValue = 20;
    emit progressChanged(progressValue);
    ret = QProcess::execute("mount", QStringList() << "-t" << "ext2" << "-o" << "loop" << tmpExt2 << "/mnt");
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Mount Failed</span>"));
        return false;
    }


    if (group == "App") {
        system(QString("rm /appdir/*").toLatin1().data());;
        QProcess::execute("mv", QStringList() << "/cfgdir/save0" << "/cfgdir/save0.upgrade");
    }

    msleep(500);

    progressValue = 30;
    emit progressChanged(progressValue);
    // ret = QProcess::execute("cp", QStringList() << "-af" << "/mnt/*" << partInfo->devn);
    ret = system(QString("cp -af /mnt/* %1").arg(partInfo->devn).toLatin1().data());
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Copy Failed</span>"));
        return false;
    }

    progressValue = 50;
    emit progressChanged(progressValue);
    ret = QProcess::execute("umount", QStringList() << "/mnt");
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Umount Failed</span>"));
        return false;
    }

    progressValue = 60;
    emit progressChanged(progressValue);
    ret = QProcess::execute("rm", QStringList() << "-f" << tmpExt2);
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Remove Failed</span>"));
        return false;
    }

    progressValue = 70;
    emit progressChanged(progressValue);
    ret = QProcess::execute("sync");
    if (ret) {
        emit statusChanged(QString("<span style='color:red;'>Sync Failed</span>"));
        return false;
    }

    progressValue = 100;
    emit progressChanged(progressValue);

    emit statusChanged("<span style='color:green;'>Success</span>");

    return true;
}

const int ReadBufferSize = 256;

const char ACK = 0x79;
const char NACK = 0x1f;
const char GetCommand = 00;
const char GetVersionCommand = 01;
const char GetIDCommand = 0x02;
const char ReadMemoryCommand = 0x11;
const char WriteMemoryCommand = 0x31;
const char EraseMemoryCommand = 0x43;
const char ExtendedEraseMemoryCommand = 0x44;

const int FlashBaseAddress = 0x08000000;

class SerialException
{
public:
    enum ErrorCode {NACK, Timeout, Unassembled, Failed};

public:
    SerialException(ErrorCode error) :
        errorCode(error)
    {

    }

    ~SerialException()
    {

    }

    ErrorCode getErrorCode() const
    {
        return errorCode;
    }

    QString getError() const
    {
        QString error;
        switch (errorCode) {
        case NACK:
            error = "NACK";
            break;
        case Timeout:
            error = "Timeout";
            break;
        case Unassembled:
            error = "Unassembled";
            break;
        case Failed:
            error = "Failed";
            break;
        default:
            break;
        }

        return error;
    }

private:
    ErrorCode errorCode;
};

QMap<int, int> IspPartition::stm32DensityMap;

IspPartition::IspPartition(const QString &group, QObject *parent) :
    Partition(group, parent),
    serial(-1)
{
    if (stm32DensityMap.empty()) {
        stm32DensityMap[0x412] = 1024;
        stm32DensityMap[0x410] = 1024;
        stm32DensityMap[0x414] = 2048;
        stm32DensityMap[0x418] = 2048;
        stm32DensityMap[0x420] = 1024;
        stm32DensityMap[0x428] = 2048;
        stm32DensityMap[0x430] = 2048;
        stm32DensityMap[0x436] = 256;
        stm32DensityMap[0x416] = 256;
    }

    readBuffer.resize(ReadBufferSize);
}

IspPartition::~IspPartition()
{
    if (serial > 0)
        ::close(serial);
}

bool IspPartition::write()
{
    serial = ::open(partInfo->devn, O_RDWR | O_NOCTTY | O_NDELAY);
    struct termios termios;
    ::tcgetattr(serial, &termios);
    termios.c_iflag = 0;
    termios.c_oflag = 0;
    termios.c_lflag = 0;
    termios.c_cflag &= ~(CSIZE | PARENB);
    termios.c_cflag |= CLOCAL | CS8;
    termios.c_cflag &= (~PARODD);
    termios.c_cflag |= PARENB;
    termios.c_cflag &= (~CSTOPB);
    termios.c_cflag &= (~CRTSCTS);
    termios.c_iflag &= (~(IXON|IXOFF|IXANY));
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;
    cfsetispeed(&termios, B57600);
    cfsetospeed(&termios, B57600);
    ::tcsetattr(serial, TCSANOW, &termios);

    emit statusChanged("<span style='color:cyan;'>Probing</span>");

    if (group == "Module") {
        Hal::instance()->setBootF1(true);
        Hal::instance()->setWDTF1(false);
    } else if (group == "IBP") {
        Hal::instance()->setBootIBP(true);
        Hal::instance()->setWDTIBP(false);
    } else {

    }

    // trigger watchdog reset
    sleep(1);
    if (group == "Module") {
        Hal::instance()->setWDTF1(true);
    } else if (group == "IBP") {
        Hal::instance()->setWDTIBP(true);
    } else {

    }

    sleep(1);

    int size;
    do {
        msleep(100);
        size = ::read(serial, readBuffer.data(), ReadBufferSize);
        // qDebug() << QByteArray(readBuffer.data(), size).toHex();
    } while (size > 0);

    bool res = true;
    devWrite(0x7f);
    try {
        devSearchAck(2);

        emit statusChanged("<span style='color:cyan;'>Updating</span>");

        qDebug() << "Get Version Command";
        devSendCommand(GetVersionCommand);
        devWaitAck();
        devWaitAck();
        qDebug() << responseBuffer.toHex();

        qDebug() << "Get ID Command";
        devSendCommand(GetIDCommand);
        devWaitAck();
        devWaitAck();
        qDebug() << responseBuffer.toHex();

        int chipId = responseBuffer.at(1) << 8 | responseBuffer.at(2);
        int density;
        if (stm32DensityMap.contains(chipId)) {
            density = stm32DensityMap.value(chipId);
            qDebug() << "Chip ID:" << chipId << ", Density:" << density;
        } else {
            qDebug() << "Cannot find density by chip id:" << chipId;
            density = 2048;
        }

        QFile module(getFilename());
        module.open(QIODevice::ReadOnly);
        const QByteArray &bin = module.readAll();
        module.close();
        int binSize = bin.size();
        int binPos = 0;

        qDebug() << "Erase Memory";
        devSendCommand(EraseMemoryCommand);
        devWaitAck();
        int numOfPages = (binSize + density - 1) / density;
        QByteArray pages;
        for (int i = 0; i < numOfPages; i++) {
            pages.append(i);
        }
        devSendData(pages);
        devWaitAck(2000);
        qDebug() << "Erase Memory Done";

        qDebug() << "Write Memory";
        do {
            // qDebug() << "Write Memory" << filePos;
            int bytes = binSize - binPos;
            bytes = bytes > 256 ? 256 : bytes;
            bytes = ((bytes + 3) / 4 ) * 4;
            QByteArray buf(256, 0xff);
            buf.replace(0, bytes, bin.mid(binPos, bytes));
            devSendCommand(WriteMemoryCommand);
            devWaitAck();
            devSendAddress(FlashBaseAddress + binPos);
            devWaitAck();
            devSendData(buf);
            devWaitAck(2000);
            binPos += bytes;
            progressValue = 100 * binPos / binSize;
            emit progressChanged(progressValue);
        } while (binPos < binSize);

        emit statusChanged("<span style='color:green;'>Success</span>");

#if 0
        qDebug() << "Read Memroy";filename.toLatin1().data(),
        QFile file("/tmp/module.bin");
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        int flashSize = 128 * 1024;
        int offset = 0;
        do {
            // qDebug() << "Read Memroy" << offset;
            int bytes = flashSize - offset;
            bytes = bytes > 256 ? 256 : bytes;
            devSendCommand(ReadMemoryCommand);
            devWaitAck();
            devSendAddress(FlashBaseAddress + offset);
            devWaitAck();
            devSendBytesRead(bytes);
            devWaitAck();
            devReceiveData(bytes);
            file.write(responseBuffer);
            offset += bytes;
        } while (offset < flashSize);
        file.close();
#endif
    } catch (SerialException exception) {
        if (exception.getErrorCode() == SerialException::Unassembled) {
            emit statusChanged(QString("<span style='color:yellow;'>%1</span>").arg(exception.getError()));
            res = true;
        } else {
            emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(exception.getError()));
            res = false;
        }
    }

    if (group == "Module") {
        Hal::instance()->setBootF1(false);
        Hal::instance()->setWDTF1(false);
    } else if (group == "IBP") {
        Hal::instance()->setBootIBP(false);
        Hal::instance()->setWDTIBP(false);
    } else {

    }

    sleep(1);

    if (group == "Module") {
        Hal::instance()->setWDTF1(true);
    } else if (group == "IBP") {
        Hal::instance()->setWDTIBP(true);
    } else {

    }

    return res;
}

void IspPartition::devSearchAck(int timeout)
{
    QTime maxTime = QTime::currentTime().addMSecs(timeout);

    do {
        int size = ::read(serial, readBuffer.data(), ReadBufferSize);
        if (size > 0) {
            qDebug() << QByteArray(readBuffer.data(), size).toHex();
            for (int i = 0; i < size; i++) {
                if (readBuffer.at(i) == ACK) {
                    return;
                } else if (readBuffer.at(i) == NACK) {
                    throw SerialException(SerialException::NACK);
                }
            }
        } else {
            if (QTime::currentTime() > maxTime) {
                throw SerialException(SerialException::Unassembled);
            }
            msleep(1);
        }
    } while (1);
}

void IspPartition::devWaitAck(int timeout)
{
    QTime maxTime = QTime::currentTime().addMSecs(timeout);
    responseBuffer.clear();

    do {
        char ch;
        int size = ::read(serial, &ch, 1);
        if (size > 0) {
            if (ch == ACK) {
                return;
            } else if (ch == NACK) {
                throw SerialException(SerialException::NACK);
            } else {
                responseBuffer.append(ch);
            }
        } else {
            if (QTime::currentTime() > maxTime) {
                throw SerialException(SerialException::Timeout);
            }
            msleep(1);
        }
    } while (1);
}

void IspPartition::devReceiveData(int bytes, int timeout)
{
    QTime maxTime = QTime::currentTime().addMSecs(timeout);
    responseBuffer.clear();

    do {
        int size = ::read(serial, readBuffer.data(), ReadBufferSize);
        if (size > 0) {
            responseBuffer.append(readBuffer.data(), size);
            if (responseBuffer.size() == bytes) {
                break;
            }
        } else {
            if (QTime::currentTime() > maxTime) {
                throw SerialException(SerialException::Timeout);
            }
            msleep(1);
        }

    } while (1);
}

void IspPartition::devSendCommand(char cmd)
{
    QByteArray byteArray;
    byteArray.append(cmd);
    byteArray.append(~cmd);
    devWrite(byteArray);
}

void IspPartition::devSendAddress(int address)
{
    QByteArray byteArray;
    byteArray.append((address >> 24) & 0xff);
    byteArray.append((address >> 16) & 0xff);
    byteArray.append((address >> 8) & 0xff);
    byteArray.append((address >> 0) & 0xff);
    byteArray.append(devChecksum(byteArray));
    devWrite(byteArray);
}

void IspPartition::devSendBytesRead(char bytes)
{
    devSendCommand(bytes - 1);
}

void IspPartition::devSendData(const QByteArray &data)
{
    int size = data.size();
    QByteArray byteArray;
    byteArray.append(size -1);
    byteArray.append(data);
    byteArray.append(devChecksum(byteArray));
    devWrite(byteArray);
}

char IspPartition::devChecksum(const QByteArray &data)
{
    char sum = 0;
    int size = data.size();
    if (size == 0)
        sum = 0;
    else if (size == 1)
        sum = ~data.at(0);
    else {
        for (int i = 0; i < size; i++) {
            sum = sum ^ data.at(i);
        }
    }
    return sum;
}

void IspPartition::devWrite(char ch)
{
    devWrite(QByteArray(1, ch));
}

void IspPartition::devWrite(const char *data, int maxSize)
{
    ::write(serial, data, maxSize);
}

void IspPartition::devWrite(const QByteArray &data)
{
    devWrite(data.data(), data.size());
}

IapPartition::IapPartition(const QString &group, QObject *parent) :
    Partition(group, parent),
    serial(-1)
{
    readBuffer.resize(ReadBufferSize);
    requestBuffer.reserve(ReadBufferSize);
    responseBuffer.reserve(ReadBufferSize);
}

IapPartition::~IapPartition()
{
    if (serial > 0)
        ::close(serial);
}

bool IapPartition::write()
{
    int res = true;

    serial = ::open(partInfo->devn, O_RDWR | O_NOCTTY | O_NDELAY);
    struct termios termios;
    ::tcgetattr(serial, &termios);
    termios.c_iflag = 0;
    termios.c_oflag = 0;
    termios.c_lflag = 0;
    termios.c_cflag &= ~(CSIZE | PARENB | PARODD);
    termios.c_cflag |= CLOCAL | CS8;
    termios.c_cflag &= (~CSTOPB);
    termios.c_cflag &= (~CRTSCTS);
    termios.c_iflag &= (~(IXON|IXOFF|IXANY));
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;
    cfsetispeed(&termios, B38400);
    cfsetospeed(&termios, B38400);
    ::tcsetattr(serial, TCSANOW, &termios);

    emit statusChanged("<span style='color:cyan;'>Probing</span>");

    int size;
    do {
        msleep(100);
        size = ::read(serial, readBuffer.data(), ReadBufferSize);
    } while (size > 0);

    progressValue = 0;
    emit progressChanged(progressValue);

#if 0
    try {
        devSendData(0x8, QByteArray(1, 25));
        devSendData(0x9, QByteArray(1, 2));
        devSendData(0x2);
        devSendData(0x3, QByteArray(1, 20));
        QByteArray string;
        string.append(16);
        string.append("hello world, abcefghijklmnopq");
        devSendData(0x6, string);
        devSendData(0x1);
        devWaitAck(10000);
        qDebug() << responseBuffer.toHex();
    } catch (SerialException exception) {
        qDebug() << "Printer test:" << exception.getError();
    }
#endif

    try {
        try {
            qDebug() << "Update Command";
            devSendData(0x31);
            devWaitAck(10000);
            if (responseBuffer.at(1) != 0x30)
                throw SerialException(SerialException::NACK);
        } catch (SerialException exception) {
            if (exception.getErrorCode() == SerialException::Timeout) {
                qDebug() << "Perhaps we already in iap mode";
            } else {
                throw;
            }
        }

        try {
            qDebug() << "Start Transfer Command";
            devSendData(0x33);
            devWaitAck(1000);
        } catch (SerialException exception) {
            if (exception.getErrorCode() == SerialException::Timeout) {
                throw SerialException(SerialException::Unassembled);
            } else {
                throw;
            }
        }

        emit statusChanged("<span style='color:cyan;'>Updating</span>");

        qDebug() << "Transfer File";
        QFile module(getFilename());
        module.open(QIODevice::ReadOnly);
        int fileSize = module.size();
        int filePos = 0;
        do {
            // qDebug() << "Transfer File" << filePos;
            int bytes = fileSize - filePos;
            bytes = bytes > 128 ? 128 : bytes;
            QByteArray fileBuf(bytes, 0xff);
            module.seek(filePos);
            module.read(fileBuf.data(), bytes);

TryAgain:
            devSendData(0x35, fileBuf);
            devWaitAck(1000);
            switch (responseBuffer.at(1)) {
            case 0x32:
                break;
            case 0x34:
                qDebug() << "Checksum error, try again";
                goto TryAgain;
                break;
            default:
                throw SerialException(SerialException::NACK);
                break;
            }

            filePos += bytes;
            progressValue = 100 * filePos / fileSize;
            emit progressChanged(progressValue);
        } while (filePos < fileSize);
        module.close();

        qDebug() << "End Transfer Command";
        devSendData(0x37);

        emit statusChanged("<span style='color:green;'>Success</span>");
    } catch (SerialException exception) {
        if (exception.getErrorCode() == SerialException::Unassembled) {
            emit statusChanged(QString("<span style='color:yellow;'>%1</span>").arg(exception.getError()));
            res = true;
        } else {
            emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(exception.getError()));
            res = false;
        }
    }

    return res;
}

void IapPartition::devSendData(char type, const QByteArray &args)
{
    int argc = args.size();
    requestBuffer.clear();
    requestBuffer.append(0x02);
    requestBuffer.append(type);
    requestBuffer.append(argc);
    if (argc > 0)
        requestBuffer.append(args);
    requestBuffer.append(devChecksum(args));
    requestBuffer.append(0x03);

#if 0
    if (argc < 32)
        qDebug() << "Host:" << requestBuffer.toHex();
    else {
        int size = requestBuffer.size();
        qDebug() << "Host:"
                 << "id" << QString::number(requestBuffer[1])
                 << "len" << QString::number(requestBuffer[2])
                 << "csum" << QString::number(requestBuffer[size - 2]);
    }
#endif
    devWrite(requestBuffer);
}

char IapPartition::devChecksum(const QByteArray &args)
{
    char sum = 0;

    for (int i = 0; i < args.size(); i++) {
        sum += args.at(i);
    }

    return sum;
}

void IapPartition::devWaitAck(int timeout)
{
    QTime maxTime = QTime::currentTime().addMSecs(timeout);
    responseBuffer.clear();

    do {
        char ch;
        int size = ::read(serial, &ch, 1);
        if (size > 0) {
            if (ch == 0x02) {
                responseBuffer.clear();
                responseBuffer.append(ch);
            } else if (ch == 0x03) {
                responseBuffer.append(ch);
                // qDebug() << "Printer:" << responseBuffer.toHex();
                if (responseBuffer.size() < 5) {
                    throw SerialException(SerialException::NACK);
                }
                unsigned char id = responseBuffer.at(1);
                if ((id == 0x70) || (id == 0x80))
                    continue;
                return;
            } else {
                responseBuffer.append(ch);
            }
        } else {
            if (QTime::currentTime() > maxTime) {
                throw SerialException(SerialException::Timeout);
            }
            msleep(1);
        }
    } while (1);
}

void IapPartition::devWrite(const char *data, int maxSize)
{
    ::write(serial, data, maxSize);
}

void IapPartition::devWrite(const QByteArray &data)
{
    devWrite(data.data(), data.size());
}

FetalIapPartition::FetalIapPartition(const QString &group, QObject *parent) :
    Partition(group, parent),
    serial(-1)
{
    readBuffer.resize(ReadBufferSize);
    requestBuffer.reserve(ReadBufferSize);
    responseBuffer.reserve(ReadBufferSize);
}

FetalIapPartition::~FetalIapPartition()
{
    closeSerial();
}

bool FetalIapPartition::write()
{
    QStringList serialDevnAvailableList;
    serialDevnAvailableList << "/dev/ttyS0";
    serialDevnAvailableList << "/dev/ttyS1";
    serialDevnAvailableList << "/dev/ttyS2";
    serialDevnAvailableList << "/dev/ttyS3";

    QStringList serialDevnActiveList;

    emit statusChanged("<span style='color:cyan;'>Probing</span>");
#if 1
    QStringListIterator devnAvailables(serialDevnAvailableList);
    while (devnAvailables.hasNext()) {
        const QString &devn = devnAvailables.next();
        openSerial(devn);
        if (probleSerial())
            serialDevnActiveList << devn;
        closeSerial();
    }
#else
    serialDevnActiveList << "/dev/ttyS0";
#endif

    int devCount = serialDevnActiveList.count();
    qDebug() << "Active serial:" << serialDevnActiveList << devCount;

    if (devCount == 0) {
        emit statusChanged("<span style='color:yellow;'>Unassembled</span>");
        return true;
    }

    int res = true;

    emit statusChanged("<span style='color:cyan;'>Updating</span>");

    progressValue = 0;
    emit progressChanged(progressValue);

    QStringListIterator devnActives(serialDevnActiveList);
    while (devnActives.hasNext()) {
        const QString &devn = devnActives.next();
        openSerial(devn);
        try {
            try {
                enterIap();
                msleep(1000);
            } catch (SerialException exception) {
                if (exception.getErrorCode() == SerialException::Timeout) {
                    qDebug() << "Perhaps we already in iap mode";
                } else {
                    throw;
                }
            }
            confirmIap();

            qDebug() << "Transfer File";
            QFile module(getFilename());
            module.open(QIODevice::ReadOnly);
            int fileSize = module.size();
            int filePos = 0;
            int bytes;
            do {
                // qDebug() << "Transfer File" << filePos;
                bytes = fileSize - filePos;
                bytes = bytes > 240 ? 240 : bytes;
                module.seek(filePos);
                QByteArray fileBuf;
                fileBuf.append(0xff);
                fileBuf.append(module.read(bytes));

                devSendData('$', fileBuf);
                devWaitAck('!');
                if (responseBuffer.count() != 7) {
                    qDebug() << "Write Flash" << responseBuffer.toHex();
                    throw SerialException(SerialException::NACK);
                }
                switch (responseBuffer.at(4)) {
                case 0:
                    break;
                case 1:
                    qDebug() << "Write Flash FAIL";
                    throw SerialException(SerialException::Failed);
                    break;
                case 2:
                    qDebug() << "Write Flash Validity FAIL";
                    break;
                default:
                    qDebug() << "Write Flash NO IAP Request";
                    throw SerialException(SerialException::NACK);
                    break;
                }

                filePos += bytes;
                progressValue = 100 * filePos / fileSize / devCount;
                emit progressChanged(progressValue);
            } while (filePos < fileSize);
            module.close();

            qDebug() << "last bytes:" << bytes;
            if ((bytes != 0) && (bytes % 8) == 0) {
                devSendData('$', QByteArray(1, 0xff));
                devWaitAck('!');
                if (responseBuffer.count() != 7) {
                    qDebug() << "Write Flash" << responseBuffer.toHex();
                    throw SerialException(SerialException::NACK);
                }
                switch (responseBuffer.at(4)) {
                case 0:
                    qDebug() << "Write Flash OK";
                    break;
                case 1:
                    qDebug() << "Write Flash FAIL";
                    break;
                case 2:
                    qDebug() << "Write Flash Validity FAIL";
                    break;
                case 3:
                    qDebug() << "Write Flash Finish";
                    break;
                default:
                    throw SerialException(SerialException::NACK);
                    break;
                }
            }

            qDebug() << "End Transfer Command";

        } catch (SerialException exception) {
            qDebug() << __FUNCTION__ << __LINE__ << exception.getError();
            closeSerial();
            emit statusChanged(QString("<span style='color:red;'>%1</span>").arg(exception.getError()));
            return false;
        }
        closeSerial();
    }
    emit statusChanged("<span style='color:green;'>Success</span>");

    return res;
}

void FetalIapPartition::openSerial(const QString &devn)
{
    qDebug() << "Fetal serial:" << devn;
    const char *dev = devn.toLatin1().constData();
    serial = ::open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    struct termios termios;
    ::tcgetattr(serial, &termios);
    termios.c_iflag = 0;
    termios.c_oflag = 0;
    termios.c_lflag = 0;
    termios.c_cflag &= ~(CSIZE | PARENB | PARODD);
    termios.c_cflag |= CLOCAL | CS8;
    termios.c_cflag &= (~CSTOPB);
    termios.c_cflag &= (~CRTSCTS);
    termios.c_iflag &= (~(IXON|IXOFF|IXANY));
    termios.c_cc[VMIN] = 1;
    termios.c_cc[VTIME] = 0;
    cfsetispeed(&termios, B460800);
    cfsetospeed(&termios, B460800);
    ::tcsetattr(serial, TCSANOW, &termios);
}

void FetalIapPartition::closeSerial()
{
    if (serial > 0) {
        ::close(serial);
        serial = -1;
    }
}

bool FetalIapPartition::probleSerial()
{
    if (serial < 0)
        return false;
    int size;
    do {
        msleep(5);
        size = ::read(serial, readBuffer.data(), ReadBufferSize);
    } while (size > 0);

    devSendData('L', QByteArray(1, 0xff));
    try {
        devWaitAck('V');
    } catch (SerialException exception) {
        qDebug() << "Probling fetal module failed" << exception.getError();
        return false;
    }
    qDebug() << "Fetal module version:" << responseBuffer;
    return true;
}

void FetalIapPartition::enterIap()
{
    int size;
    do {
        msleep(5);
        size = ::read(serial, readBuffer.data(), ReadBufferSize);
    } while (size > 0);

    QByteArray params;
    params.append(0xff);
    params.append("IAP");
    devSendData('@', params);
    devWaitAck('?');
    devSendData('@', params);
    qDebug() << "Jump to IAP" << responseBuffer.toHex();
}

void FetalIapPartition::confirmIap()
{
    int size;
    do {
        msleep(5);
        size = ::read(serial, readBuffer.data(), ReadBufferSize);
    } while (size > 0);

    QByteArray params;
    params.append(0xff);
    params.append("IAP");
    devSendData(':', params);
    devWaitAck('#', 16000);
    qDebug() << "Confirm IAP:" << responseBuffer.toHex();
    if (responseBuffer.count() != 7 || responseBuffer.at(4) != 0) {
        // exception
    }
}

void FetalIapPartition::devSendData(char type, const QByteArray &args)
{
    int argc = args.size();
    requestBuffer.clear();
    requestBuffer.append(0x02);
    requestBuffer.append(type);
    requestBuffer.append(argc);
    if (argc > 0)
        requestBuffer.append(args);
    requestBuffer.append(devChecksum(args));
    requestBuffer.append(0x03);

#if 0
    if (argc < 32)
        qDebug() << "Host:" << requestBuffer.toHex();
    else {
        int size = requestBuffer.size();
        qDebug() << "Host:"
                 << "id" << QString::number(requestBuffer[1])
                 << "len" << QString::number(requestBuffer[2])
                 << "csum" << QString::number(requestBuffer[size - 2]);
    }
#endif
    devWrite(requestBuffer);
}

char FetalIapPartition::devChecksum(const QByteArray &args)
{
    char sum = 0;

    for (int i = 0; i < args.size(); i++) {
        sum += args.at(i);
    }

    return sum;
}

void FetalIapPartition::devWaitAck(char ack, int timeout)
{
    QTime maxTime = QTime::currentTime().addMSecs(timeout);
    responseBuffer.clear();

    do {
        char buf[128];
        int size = ::read(serial, buf, 128);
        if (size > 0) {
            for (int i = 0; i < size; i++) {
                char ch = buf[i];
                if (ch == 0x03) {
                    responseBuffer.append(ch);
                    int size = responseBuffer.size();
                    if (size < 6) {
                        continue;
                    } else {
                        do {
                            int size = responseBuffer.size();
                            char stx = responseBuffer.at(0);
                            if (stx != 0x02) {
                                responseBuffer.remove(0, 1);
                            } else if (size >= 6){
                                char len = responseBuffer.at(2);
                                if (len == (size - 5)) {
                                    char id = responseBuffer.at(1);
                                    if ((id == ack)) {
                                        return;
                                    } else {
                                        break;
                                    }
                                } else {
                                    responseBuffer.remove(0, 1);
                                }
                            } else {
                                break;
                            }
                        } while (responseBuffer.size() > 0);
                    }
                } else {
                    responseBuffer.append(ch);
                }
            }
        } else {
            if (QTime::currentTime() > maxTime) {
                throw SerialException(SerialException::Timeout);
            }
            msleep(5);
        }
    } while (1);
}

void FetalIapPartition::devWrite(const char *data, int maxSize)
{
    ::write(serial, data, maxSize);
}

void FetalIapPartition::devWrite(const QByteArray &data)
{
    devWrite(data.data(), data.size());
}
