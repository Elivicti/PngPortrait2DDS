#include "PngPortrait2DDS.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PngPortrait2DDS w;
    w.show();
    return a.exec();
}
