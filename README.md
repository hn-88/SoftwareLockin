# SoftwareLockin

Made slight changes to make the lockin code from
https://leso.epfl.ch/cms/site/leso/lang/fr/pcsc-lockin
http://leso2.epfl.ch/downloads/PCSC-Lockin/lockin.tar.gz

Code is originally written for QT4. To build it on QT5, the following changes were made:

1. edited the pro files to add the line
QT += widgets

2. edited vumeter.h etc
changing QtGui/QWidget to QWidget 

3. changed all references of toAscii() to toLatin1() in lockin.cpp
and dockoutput.cpp

as per
http://doc.qt.io/qt-5/qstring-obsolete.html
