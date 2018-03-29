/********************************************************************************
** Form generated from reading UI file 'dockoutput.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOCKOUTPUT_H
#define UI_DOCKOUTPUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DockOutput
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *clear;
    QToolButton *save;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *text;

    void setupUi(QWidget *DockOutput)
    {
        if (DockOutput->objectName().isEmpty())
            DockOutput->setObjectName(QStringLiteral("DockOutput"));
        DockOutput->resize(400, 300);
        verticalLayout = new QVBoxLayout(DockOutput);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        clear = new QToolButton(DockOutput);
        clear->setObjectName(QStringLiteral("clear"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/broom-icon.svg"), QSize(), QIcon::Normal, QIcon::Off);
        clear->setIcon(icon);

        horizontalLayout->addWidget(clear);

        save = new QToolButton(DockOutput);
        save->setObjectName(QStringLiteral("save"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/save-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        save->setIcon(icon1);

        horizontalLayout->addWidget(save);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        text = new QPlainTextEdit(DockOutput);
        text->setObjectName(QStringLiteral("text"));

        verticalLayout->addWidget(text);


        retranslateUi(DockOutput);

        QMetaObject::connectSlotsByName(DockOutput);
    } // setupUi

    void retranslateUi(QWidget *DockOutput)
    {
        DockOutput->setWindowTitle(QApplication::translate("DockOutput", "Form", 0));
        clear->setText(QApplication::translate("DockOutput", "clear", 0));
        save->setText(QApplication::translate("DockOutput", "save", 0));
    } // retranslateUi

};

namespace Ui {
    class DockOutput: public Ui_DockOutput {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOCKOUTPUT_H
