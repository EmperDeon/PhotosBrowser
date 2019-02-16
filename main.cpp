/*
	Copyright (c) 2017-2019 by Ilya Barykin
	Released under the MIT License.
	See the provided LICENSE.TXT file for details.
*/

#include <QtCore/qlogging.h>
#include <QtWidgets/QApplication>
#include <QtGui/QFontDatabase>
#include "Window.h"


void qtMessageHandler(QtMsgType type, const QMessageLogContext &cont, const QString &msg) {
    Q_UNUSED(cont)
    switch (type) {
        default:
            printf("%s \n", msg.toStdString().c_str());
            break;
    }
}

int main(int argv, char **argc) {
    QApplication a(argv, argc);
    qInstallMessageHandler(qtMessageHandler);

    QCoreApplication::setOrganizationName("IlzSoft");
    QCoreApplication::setOrganizationDomain("github.com/emperdeon");
    QCoreApplication::setApplicationName("PhotosBrowser");
    QFontDatabase::addApplicationFont(":/fonts/PT_Sans-Web-Regular.ttf");
    qApp->setWindowIcon(QIcon(":/i128.ico"));

    QStringList l = QApplication::arguments();
    Window w(l.size() == 2 ? l[1] : "");
    w.showFullScreen();

    w.reload();

    return QApplication::exec();
}

void operator delete(void *p, std::size_t) {
    std::free(p);
}

void operator delete[](void *p, std::size_t) {
    std::free(p);
}
