#include "MainWindow.h"
#include "LoginWindow.h"
#include "ApiClient.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ApiClient api;
    LoginWindow login(&api);
    
    if (login.exec() == QDialog::Accepted) {
        MainWindow w(&api);
        w.show();
        return a.exec();
    }
    
    return 0;
}