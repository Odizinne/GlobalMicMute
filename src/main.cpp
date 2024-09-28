#include "gmm.h"
#include "utils.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    if (Utils::isWindows10()) {
        a.setStyle("fusion");
    }
    GMM w;
    return a.exec();
}
