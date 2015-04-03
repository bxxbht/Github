/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Thu Mar 26 16:58:06 2015
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *versionComboBox;
    QPushButton *removeVersionPushButton;
    QLabel *label_3;
    QLineEdit *descriptionLineEdit;
    QLabel *label_5;
    QComboBox *platformComboBox;
    QGroupBox *sectionGroupBox;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *outputGroupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *usbDiskRadioButton;
    QRadioButton *selfExtractingFormatRadioButton;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QComboBox *outputDirectoryComboBox;
    QPushButton *removeOutputDirPushButton;
    QPushButton *browsePushButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(800, 600);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        versionComboBox = new QComboBox(groupBox);
        versionComboBox->setObjectName(QString::fromUtf8("versionComboBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(versionComboBox->sizePolicy().hasHeightForWidth());
        versionComboBox->setSizePolicy(sizePolicy);
        versionComboBox->setEditable(true);
        versionComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);

        horizontalLayout_3->addWidget(versionComboBox);

        removeVersionPushButton = new QPushButton(groupBox);
        removeVersionPushButton->setObjectName(QString::fromUtf8("removeVersionPushButton"));

        horizontalLayout_3->addWidget(removeVersionPushButton);


        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout_3);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        descriptionLineEdit = new QLineEdit(groupBox);
        descriptionLineEdit->setObjectName(QString::fromUtf8("descriptionLineEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, descriptionLineEdit);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        platformComboBox = new QComboBox(groupBox);
        platformComboBox->setObjectName(QString::fromUtf8("platformComboBox"));

        formLayout->setWidget(3, QFormLayout::FieldRole, platformComboBox);


        verticalLayout->addWidget(groupBox);

        sectionGroupBox = new QGroupBox(Dialog);
        sectionGroupBox->setObjectName(QString::fromUtf8("sectionGroupBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(sectionGroupBox->sizePolicy().hasHeightForWidth());
        sectionGroupBox->setSizePolicy(sizePolicy1);
        verticalLayout_3 = new QVBoxLayout(sectionGroupBox);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

        verticalLayout->addWidget(sectionGroupBox);

        outputGroupBox = new QGroupBox(Dialog);
        outputGroupBox->setObjectName(QString::fromUtf8("outputGroupBox"));
        verticalLayout_2 = new QVBoxLayout(outputGroupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        usbDiskRadioButton = new QRadioButton(outputGroupBox);
        usbDiskRadioButton->setObjectName(QString::fromUtf8("usbDiskRadioButton"));
        usbDiskRadioButton->setChecked(false);

        horizontalLayout_2->addWidget(usbDiskRadioButton);

        selfExtractingFormatRadioButton = new QRadioButton(outputGroupBox);
        selfExtractingFormatRadioButton->setObjectName(QString::fromUtf8("selfExtractingFormatRadioButton"));
        selfExtractingFormatRadioButton->setChecked(true);

        horizontalLayout_2->addWidget(selfExtractingFormatRadioButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(outputGroupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        outputDirectoryComboBox = new QComboBox(outputGroupBox);
        outputDirectoryComboBox->setObjectName(QString::fromUtf8("outputDirectoryComboBox"));
        sizePolicy.setHeightForWidth(outputDirectoryComboBox->sizePolicy().hasHeightForWidth());
        outputDirectoryComboBox->setSizePolicy(sizePolicy);
        outputDirectoryComboBox->setEditable(true);
        outputDirectoryComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);

        horizontalLayout->addWidget(outputDirectoryComboBox);

        removeOutputDirPushButton = new QPushButton(outputGroupBox);
        removeOutputDirPushButton->setObjectName(QString::fromUtf8("removeOutputDirPushButton"));

        horizontalLayout->addWidget(removeOutputDirPushButton);

        browsePushButton = new QPushButton(outputGroupBox);
        browsePushButton->setObjectName(QString::fromUtf8("browsePushButton"));

        horizontalLayout->addWidget(browsePushButton);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(outputGroupBox);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Help|QDialogButtonBox::Save);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(close()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\">Lakshi</span></p><p align=\"center\">Update Package Manager</p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Dialog", "Options", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Dialog", "Version", 0, QApplication::UnicodeUTF8));
        removeVersionPushButton->setText(QApplication::translate("Dialog", "Remove", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Dialog", "Description", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Dialog", "Platform", 0, QApplication::UnicodeUTF8));
        sectionGroupBox->setTitle(QApplication::translate("Dialog", "Partitions", 0, QApplication::UnicodeUTF8));
        outputGroupBox->setTitle(QApplication::translate("Dialog", "Output", 0, QApplication::UnicodeUTF8));
        usbDiskRadioButton->setText(QApplication::translate("Dialog", "USB Disk", 0, QApplication::UnicodeUTF8));
        selfExtractingFormatRadioButton->setText(QApplication::translate("Dialog", "Self-extracting", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "Directory", 0, QApplication::UnicodeUTF8));
        removeOutputDirPushButton->setText(QApplication::translate("Dialog", "Remove", 0, QApplication::UnicodeUTF8));
        browsePushButton->setText(QApplication::translate("Dialog", "Browse", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
