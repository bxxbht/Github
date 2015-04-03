/********************************************************************************
** Form generated from reading UI file 'partition.ui'
**
** Created: Thu Nov 20 16:57:33 2014
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARTITION_H
#define UI_PARTITION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Partition
{
public:
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox;
    QLineEdit *versionLineEdit;
    QLineEdit *lineEdit;
    QPushButton *pushButton;

    void setupUi(QWidget *Partition)
    {
        if (Partition->objectName().isEmpty())
            Partition->setObjectName(QString::fromUtf8("Partition"));
        Partition->resize(640, 64);
        horizontalLayout = new QHBoxLayout(Partition);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        checkBox = new QCheckBox(Partition);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setMinimumSize(QSize(128, 0));

        horizontalLayout->addWidget(checkBox);

        versionLineEdit = new QLineEdit(Partition);
        versionLineEdit->setObjectName(QString::fromUtf8("versionLineEdit"));
        versionLineEdit->setMaximumSize(QSize(160, 16777215));
        versionLineEdit->setReadOnly(true);

        horizontalLayout->addWidget(versionLineEdit);

        lineEdit = new QLineEdit(Partition);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setReadOnly(true);

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(Partition);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);


        retranslateUi(Partition);
        QObject::connect(checkBox, SIGNAL(clicked(bool)), lineEdit, SLOT(setEnabled(bool)));
        QObject::connect(checkBox, SIGNAL(clicked(bool)), pushButton, SLOT(setEnabled(bool)));
        QObject::connect(checkBox, SIGNAL(clicked(bool)), versionLineEdit, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(Partition);
    } // setupUi

    void retranslateUi(QWidget *Partition)
    {
        Partition->setWindowTitle(QApplication::translate("Partition", "Form", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("Partition", "CheckBox", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Partition", "Browse", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Partition: public Ui_Partition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARTITION_H
