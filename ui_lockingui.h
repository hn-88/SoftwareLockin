/********************************************************************************
** Form generated from reading UI file 'lockingui.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOCKINGUI_H
#define UI_LOCKINGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "vumeter.h"

QT_BEGIN_NAMESPACE

class Ui_LockinGui
{
public:
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QLabel *label_8;
    QComboBox *combodevice;
    QHBoxLayout *horizontalLayout_8;
    QSpinBox *rate;
    QLabel *label_12;
    QComboBox *combooutput;
    QPushButton *startstop;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QProgressBar *progressBarX;
    QLabel *labelpowx;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_3;
    QLabel *chopperfrequency;
    Vumeter *vumeterInput;
    QLabel *label_9;
    Vumeter *vumeterOutput;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QProgressBar *progressBarY;
    QLabel *labelpowy;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelx;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_4;
    QSpinBox *noisesamples;
    QLabel *noise;
    QSpacerItem *horizontalSpacer;
    QLabel *labely;
    QTabWidget *tabWidget;
    QWidget *tab_param;
    QVBoxLayout *verticalLayout_14;
    QFormLayout *formLayout;
    QLabel *label_5;
    QDoubleSpinBox *integrationtime;
    QLabel *label_7;
    QLabel *label_13;
    QComboBox *lowpassmode;
    QLabel *label_2;
    QComboBox *pllshape;
    QSpacerItem *verticalSpacer_2;
    QDoubleSpinBox *outputfrequency;
    QWidget *tab_phase;
    QVBoxLayout *verticalLayout_13;
    QDial *dial;
    QHBoxLayout *horizontalLayout_6;
    QToolButton *minusninety;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *autophase;
    QSpacerItem *horizontalSpacer_7;
    QToolButton *plusninety;
    QDoubleSpinBox *phi;
    QWidget *tab_ampli;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_9;
    QProgressBar *progressBarAmp;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *manualrange;
    QVBoxLayout *verticalLayout_8;
    QComboBox *comborange;
    QSpacerItem *verticalSpacer;
    QPushButton *editranges;
    QLabel *labelsaturation;

    void setupUi(QWidget *LockinGui)
    {
        if (LockinGui->objectName().isEmpty())
            LockinGui->setObjectName(QStringLiteral("LockinGui"));
        LockinGui->resize(430, 680);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/samovar-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        LockinGui->setWindowIcon(icon);
        verticalLayout_6 = new QVBoxLayout(LockinGui);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        groupBox_3 = new QGroupBox(LockinGui);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_6);

        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_8);

        combodevice = new QComboBox(groupBox_3);
        combodevice->setObjectName(QStringLiteral("combodevice"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, combodevice);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        rate = new QSpinBox(groupBox_3);
        rate->setObjectName(QStringLiteral("rate"));
        rate->setMaximum(999999);
        rate->setSingleStep(1000);
        rate->setValue(44100);

        horizontalLayout_8->addWidget(rate);


        formLayout_2->setLayout(2, QFormLayout::FieldRole, horizontalLayout_8);

        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_12);

        combooutput = new QComboBox(groupBox_3);
        combooutput->setObjectName(QStringLiteral("combooutput"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, combooutput);


        verticalLayout_4->addLayout(formLayout_2);

        startstop = new QPushButton(groupBox_3);
        startstop->setObjectName(QStringLiteral("startstop"));

        verticalLayout_4->addWidget(startstop);


        verticalLayout_6->addWidget(groupBox_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(LockinGui);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setPointSize(17);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        progressBarX = new QProgressBar(LockinGui);
        progressBarX->setObjectName(QStringLiteral("progressBarX"));
        progressBarX->setMaximum(1000);
        progressBarX->setValue(1000);
        progressBarX->setTextVisible(false);
        progressBarX->setOrientation(Qt::Vertical);

        horizontalLayout_2->addWidget(progressBarX);


        verticalLayout_2->addLayout(horizontalLayout_2);

        labelpowx = new QLabel(LockinGui);
        labelpowx->setObjectName(QStringLiteral("labelpowx"));
        labelpowx->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(labelpowx);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_10 = new QLabel(LockinGui);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_7->addWidget(label_10);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);

        chopperfrequency = new QLabel(LockinGui);
        chopperfrequency->setObjectName(QStringLiteral("chopperfrequency"));

        horizontalLayout_7->addWidget(chopperfrequency);


        verticalLayout->addLayout(horizontalLayout_7);

        vumeterInput = new Vumeter(LockinGui);
        vumeterInput->setObjectName(QStringLiteral("vumeterInput"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vumeterInput->sizePolicy().hasHeightForWidth());
        vumeterInput->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(vumeterInput);

        label_9 = new QLabel(LockinGui);
        label_9->setObjectName(QStringLiteral("label_9"));

        verticalLayout->addWidget(label_9);

        vumeterOutput = new Vumeter(LockinGui);
        vumeterOutput->setObjectName(QStringLiteral("vumeterOutput"));
        sizePolicy.setHeightForWidth(vumeterOutput->sizePolicy().hasHeightForWidth());
        vumeterOutput->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(vumeterOutput);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_3 = new QLabel(LockinGui);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        progressBarY = new QProgressBar(LockinGui);
        progressBarY->setObjectName(QStringLiteral("progressBarY"));
        progressBarY->setMaximum(1000);
        progressBarY->setValue(1000);
        progressBarY->setTextVisible(false);
        progressBarY->setOrientation(Qt::Vertical);

        horizontalLayout->addWidget(progressBarY);


        verticalLayout_3->addLayout(horizontalLayout);

        labelpowy = new QLabel(LockinGui);
        labelpowy->setObjectName(QStringLiteral("labelpowy"));
        labelpowy->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(labelpowy);


        horizontalLayout_3->addLayout(verticalLayout_3);


        verticalLayout_6->addLayout(horizontalLayout_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        labelx = new QLabel(LockinGui);
        labelx->setObjectName(QStringLiteral("labelx"));
        labelx->setMinimumSize(QSize(100, 0));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        labelx->setFont(font1);

        horizontalLayout_5->addWidget(labelx);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        label_4 = new QLabel(LockinGui);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font1);

        horizontalLayout_5->addWidget(label_4);

        noisesamples = new QSpinBox(LockinGui);
        noisesamples->setObjectName(QStringLiteral("noisesamples"));
        noisesamples->setMinimum(1);
        noisesamples->setMaximum(1000);
        noisesamples->setValue(10);

        horizontalLayout_5->addWidget(noisesamples);

        noise = new QLabel(LockinGui);
        noise->setObjectName(QStringLiteral("noise"));
        noise->setMinimumSize(QSize(50, 0));
        noise->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(noise);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        labely = new QLabel(LockinGui);
        labely->setObjectName(QStringLiteral("labely"));
        labely->setMinimumSize(QSize(100, 0));
        labely->setFont(font1);

        horizontalLayout_5->addWidget(labely);


        verticalLayout_6->addLayout(horizontalLayout_5);

        tabWidget = new QTabWidget(LockinGui);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_param = new QWidget();
        tab_param->setObjectName(QStringLiteral("tab_param"));
        verticalLayout_14 = new QVBoxLayout(tab_param);
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_5 = new QLabel(tab_param);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_5);

        integrationtime = new QDoubleSpinBox(tab_param);
        integrationtime->setObjectName(QStringLiteral("integrationtime"));
        integrationtime->setDecimals(3);
        integrationtime->setSingleStep(0.1);
        integrationtime->setValue(1);

        formLayout->setWidget(0, QFormLayout::FieldRole, integrationtime);

        label_7 = new QLabel(tab_param);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_7);

        label_13 = new QLabel(tab_param);
        label_13->setObjectName(QStringLiteral("label_13"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_13);

        lowpassmode = new QComboBox(tab_param);
        lowpassmode->setObjectName(QStringLiteral("lowpassmode"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lowpassmode);

        label_2 = new QLabel(tab_param);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        pllshape = new QComboBox(tab_param);
        pllshape->setObjectName(QStringLiteral("pllshape"));

        formLayout->setWidget(4, QFormLayout::FieldRole, pllshape);

        verticalSpacer_2 = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(3, QFormLayout::LabelRole, verticalSpacer_2);

        outputfrequency = new QDoubleSpinBox(tab_param);
        outputfrequency->setObjectName(QStringLiteral("outputfrequency"));
        outputfrequency->setEnabled(true);
        outputfrequency->setMinimum(0.01);
        outputfrequency->setValue(5);

        formLayout->setWidget(1, QFormLayout::FieldRole, outputfrequency);


        verticalLayout_14->addLayout(formLayout);

        tabWidget->addTab(tab_param, QString());
        tab_phase = new QWidget();
        tab_phase->setObjectName(QStringLiteral("tab_phase"));
        verticalLayout_13 = new QVBoxLayout(tab_phase);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        dial = new QDial(tab_phase);
        dial->setObjectName(QStringLiteral("dial"));
        dial->setEnabled(true);
        dial->setMaximum(360);
        dial->setPageStep(15);
        dial->setValue(0);
        dial->setTracking(true);
        dial->setInvertedAppearance(false);
        dial->setInvertedControls(false);
        dial->setWrapping(true);
        dial->setNotchTarget(15);
        dial->setNotchesVisible(true);

        verticalLayout_13->addWidget(dial);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        minusninety = new QToolButton(tab_phase);
        minusninety->setObjectName(QStringLiteral("minusninety"));

        horizontalLayout_6->addWidget(minusninety);

        horizontalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        autophase = new QToolButton(tab_phase);
        autophase->setObjectName(QStringLiteral("autophase"));
        autophase->setMinimumSize(QSize(55, 0));

        horizontalLayout_6->addWidget(autophase);

        horizontalSpacer_7 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        plusninety = new QToolButton(tab_phase);
        plusninety->setObjectName(QStringLiteral("plusninety"));

        horizontalLayout_6->addWidget(plusninety);


        verticalLayout_13->addLayout(horizontalLayout_6);

        phi = new QDoubleSpinBox(tab_phase);
        phi->setObjectName(QStringLiteral("phi"));
        phi->setDecimals(3);
        phi->setMinimum(-360);
        phi->setMaximum(360);
        phi->setSingleStep(0.1);

        verticalLayout_13->addWidget(phi);

        tabWidget->addTab(tab_phase, QString());
        tab_ampli = new QWidget();
        tab_ampli->setObjectName(QStringLiteral("tab_ampli"));
        verticalLayout_12 = new QVBoxLayout(tab_ampli);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        progressBarAmp = new QProgressBar(tab_ampli);
        progressBarAmp->setObjectName(QStringLiteral("progressBarAmp"));
        progressBarAmp->setMaximum(1000);
        progressBarAmp->setValue(1000);
        progressBarAmp->setTextVisible(true);
        progressBarAmp->setOrientation(Qt::Vertical);

        horizontalLayout_9->addWidget(progressBarAmp);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        manualrange = new QGroupBox(tab_ampli);
        manualrange->setObjectName(QStringLiteral("manualrange"));
        manualrange->setCheckable(true);
        verticalLayout_8 = new QVBoxLayout(manualrange);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        comborange = new QComboBox(manualrange);
        comborange->setObjectName(QStringLiteral("comborange"));

        verticalLayout_8->addWidget(comborange);


        verticalLayout_10->addWidget(manualrange);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer);

        editranges = new QPushButton(tab_ampli);
        editranges->setObjectName(QStringLiteral("editranges"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/chart-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        editranges->setIcon(icon1);

        verticalLayout_10->addWidget(editranges);


        horizontalLayout_9->addLayout(verticalLayout_10);


        verticalLayout_12->addLayout(horizontalLayout_9);

        labelsaturation = new QLabel(tab_ampli);
        labelsaturation->setObjectName(QStringLiteral("labelsaturation"));

        verticalLayout_12->addWidget(labelsaturation);

        tabWidget->addTab(tab_ampli, QString());

        verticalLayout_6->addWidget(tabWidget);

        QWidget::setTabOrder(integrationtime, outputfrequency);
        QWidget::setTabOrder(outputfrequency, minusninety);
        QWidget::setTabOrder(minusninety, plusninety);

        retranslateUi(LockinGui);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LockinGui);
    } // setupUi

    void retranslateUi(QWidget *LockinGui)
    {
        LockinGui->setWindowTitle(QApplication::translate("LockinGui", "PCSC-Lockin", 0));
        groupBox_3->setTitle(QApplication::translate("LockinGui", "Sound card", 0));
        label_6->setText(QApplication::translate("LockinGui", "input", 0));
        label_8->setText(QApplication::translate("LockinGui", "rate", 0));
        combodevice->clear();
        combodevice->insertItems(0, QStringList()
         << QApplication::translate("LockinGui", "Select a device", 0)
        );
        rate->setSuffix(QApplication::translate("LockinGui", " Hz", 0));
        label_12->setText(QApplication::translate("LockinGui", "output", 0));
        combooutput->clear();
        combooutput->insertItems(0, QStringList()
         << QApplication::translate("LockinGui", "Select a device or leave empty", 0)
        );
        startstop->setText(QApplication::translate("LockinGui", "open", 0));
        label->setText(QApplication::translate("LockinGui", "X", 0));
        labelpowx->setText(QString());
        label_10->setText(QApplication::translate("LockinGui", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Phased signals : <span style=\" font-weight:600; color:#ff0000;\">measure</span> &amp; <span style=\" font-weight:600; color:#6e6e6e;\">pll</span></p></body></html>", 0));
        chopperfrequency->setText(QApplication::translate("LockinGui", "Hz", 0));
        label_9->setText(QApplication::translate("LockinGui", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Output lockin : <span style=\" font-weight:600; color:#ff0000;\">X</span> &amp; <span style=\" font-weight:600; color:#6e6e6e;\">Y</span></p></body></html>", 0));
        label_3->setText(QApplication::translate("LockinGui", "Y", 0));
        labelpowy->setText(QString());
        labelx->setText(QApplication::translate("LockinGui", "0.0", 0));
        label_4->setText(QApplication::translate("LockinGui", "Noise", 0));
        noise->setText(QApplication::translate("LockinGui", "...", 0));
        labely->setText(QApplication::translate("LockinGui", "0.0", 0));
        label_5->setText(QApplication::translate("LockinGui", "Integration time", 0));
        integrationtime->setSuffix(QApplication::translate("LockinGui", " s", 0));
        label_7->setText(QApplication::translate("LockinGui", "Output frequency", 0));
        label_13->setText(QApplication::translate("LockinGui", "Low-pass filter", 0));
        lowpassmode->clear();
        lowpassmode->insertItems(0, QStringList()
         << QApplication::translate("LockinGui", "AVG mode", 0)
         << QApplication::translate("LockinGui", "RC mode", 0)
        );
        label_2->setText(QApplication::translate("LockinGui", "PLL shape", 0));
        pllshape->clear();
        pllshape->insertItems(0, QStringList()
         << QApplication::translate("LockinGui", "Sine", 0)
         << QApplication::translate("LockinGui", "Square", 0)
         << QApplication::translate("LockinGui", "Triangle", 0)
        );
        outputfrequency->setSuffix(QApplication::translate("LockinGui", " Hz", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_param), QApplication::translate("LockinGui", "Parameters", 0));
        minusninety->setText(QApplication::translate("LockinGui", "-90\302\260", 0));
        autophase->setText(QApplication::translate("LockinGui", "auto", 0));
        plusninety->setText(QApplication::translate("LockinGui", "+90\302\260", 0));
        phi->setSuffix(QApplication::translate("LockinGui", "\302\260", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_phase), QApplication::translate("LockinGui", "Phase", 0));
        manualrange->setTitle(QApplication::translate("LockinGui", "Manual range", 0));
        editranges->setText(QApplication::translate("LockinGui", "Edit ranges", 0));
        labelsaturation->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_ampli), QApplication::translate("LockinGui", "Amplification", 0));
    } // retranslateUi

};

namespace Ui {
    class LockinGui: public Ui_LockinGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOCKINGUI_H
