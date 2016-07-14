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

#ifndef PARTITION_H
#define PARTITION_H

#include <QThread>
#include <QMap>

class Partition;
class DummyPartition;
class MmcPartition;
class Ext2Partition;
class IspPartition;
class IapPartition;
class FetalIapPartition;

class PartitionPackage
{
public:
    int index;
    QString version;
    QList<Partition *> partitions;

    PartitionPackage(int index, const QString& version) :
        index(index),
        version(version)
    {

    }

    void insert(Partition *part)
    {
        partitions.append(part);
    }

    int count()
    {
        return partitions.count();
    }
};

class Partition : public QThread
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(Error)

public:
    enum Status { Success, Failed };
    enum Error { Malformed, Incomplted, FileNotExist, CrcError };


    explicit Partition(const QString &group, QObject *parent = 0);
    ~Partition();

    static void mapGroup();
    static PartitionPackage *createPackage(int index, const QString &version);
    static Partition *createPartition(const QString &group);
    static PartitionPackage *insertPackage(int index, const QString &directory);
    static bool validatePackage(int index);
    static PartitionPackage *package(int index);

    virtual bool write() = 0;

    void setFileInfo(const QString &filename, const QString newVersion, const QString &oldVersion, bool validate, uint crc)
    {
        this->filename = filename;
        this->newVersion = newVersion;
        this->oldVersion = oldVersion;
        this->validate = validate;
        this->crc = crc;
    }

    QString getGroup() const
    {
        return group;
    }

    QString getFilename() const
    {
        return filename;
    }

    QString getNewVersion() const
    {
        return newVersion;
    }

    QString getOldVersion();
    static QString getOldVersion(const QString &group);

    void setStatus(Status status)
    {
        this->status = status;
    }

    Status getStatus() const
    {
        return status;
    }

    void setError(Error error, const QString &details = "")
    {
        this->error = error;
        this->errorDetails = details;
    }

    QString getErrorDetails() const
    {
        return errorDetails;
    }

signals:
    void progressChanged(int value);
    void statusChanged(const QString &status);

protected:
    virtual void run();

private:
    Q_DISABLE_COPY(Partition)

protected:
    enum Type { Dummy, MMC, Ext2, ISP, IAP, FetalIAP };

    struct PartInfo {
    public:
        PartInfo(const char *name, int offset, int maxSize,
                const char *devn = "/dev/mmcblk0", Type type = MMC) {
            this->name = name;
            this->offset = offset;
            this->maxSize = maxSize;
            this->devn = devn;
            this->type = type;
        }

        PartInfo(const char *name, int maxSize, const char *devn, Type type = Ext2) {
            this->name = name;
            this->maxSize = maxSize;
            this->devn = devn;
            this->type = type;
        }

        const char *name;
        const char *devn;
        int offset;
        int maxSize;
        Type type;
    };

    static QMap<QString, PartInfo *> partInfoMap;

    static QList<PartitionPackage *> partitionPackages;

protected:
    QString group;
    PartInfo *partInfo;

    QString filename;
    QString newVersion;
    QString oldVersion;
    bool validate;
    uint crc;

    Status status;
    Error error;
    QString errorDetails;

    int progressValue;
};

class DummyPartition : public Partition
{
    Q_OBJECT

public:
    explicit DummyPartition(const QString &group, QObject *parent = 0);
    ~DummyPartition();

    virtual bool write();

private:
    Q_DISABLE_COPY(DummyPartition)
};

class MmcPartition : public Partition
{
    Q_OBJECT

public:
    explicit MmcPartition(const QString &group, QObject *parent = 0);
    ~MmcPartition();

    virtual bool write();

private:
    Q_DISABLE_COPY(MmcPartition)
    void destoryenv();
};

class Ext2Partition : public Partition
{
    Q_OBJECT

public:
    explicit Ext2Partition(const QString &group, QObject *parent = 0);
    ~Ext2Partition();

    virtual bool write();

private:
    Q_DISABLE_COPY(Ext2Partition)
};

class IspPartition : public Partition
{
    Q_OBJECT

public:
    explicit IspPartition(const QString &group, QObject *parent = 0);
    ~IspPartition();

    virtual bool write();

private:
    Q_DISABLE_COPY(IspPartition)
    void devSendCommand(char cmd);
    void devSendAddress(int address);
    void devSendBytesRead(char bytes);
    void devSendData(const QByteArray &data);
    char devChecksum(const QByteArray &data);
    void devWrite(char ch);
    void devWrite(const char *data, int maxSize);
    void devWrite(const QByteArray &data);
    void devSearchAck(int timeout = 50);
    void devWaitAck(int timeout = 50);
    void devReceiveData(int bytes, int timeout = 500);

private:
    static QMap<int, int> stm32DensityMap;

    int serial;
    QByteArray readBuffer;
    QByteArray responseBuffer;
};

class IapPartition : public Partition
{
    Q_OBJECT

public:
    explicit IapPartition(const QString &group, QObject *parent = 0);
    ~IapPartition();

    virtual bool write();

private:
    Q_DISABLE_COPY(IapPartition)
    void devSendData(char type, const QByteArray &args = QByteArray());
    char devChecksum(const QByteArray &args);
    void devWaitAck(int timeout = 500);
    void devWrite(const char *data, int maxSize);
    void devWrite(const QByteArray &data);

private:
    int serial;
    QByteArray readBuffer;
    QByteArray requestBuffer;
    QByteArray responseBuffer;
};

class FetalIapPartition : public Partition
{
    Q_OBJECT

public:
    explicit FetalIapPartition(const QString &group, QObject *parent = 0);
    ~FetalIapPartition();

    virtual bool write();

private:
    Q_DISABLE_COPY(FetalIapPartition)
    void openSerial(const QString &devn);
    void closeSerial();
    bool probleSerial();
    void enterIap();
    void confirmIap();
    void devSendData(char type, const QByteArray &args = QByteArray());
    char devChecksum(const QByteArray &args);
    void devWaitAck(char ack, int timeout = 500);
    void devWrite(const char *data, int maxSize);
    void devWrite(const QByteArray &data);

private:
    int serial;
    QByteArray readBuffer;
    QByteArray requestBuffer;
    QByteArray responseBuffer;
};

#endif // PARTITION_H
