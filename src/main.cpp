#include "MainWindow.h"
#include "LoginWindow.h"
#include "ApiClient.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    a.setWindowIcon(QIcon(":/icons/logo.png"));
    
    a.setApplicationName("OneCT Messenger");
    a.setApplicationDisplayName("OneCT Messenger");
    a.setOrganizationName("OneCT");
    
    ApiClient api;
    LoginWindow login(&api);
    
    if (login.exec() == QDialog::Accepted) {
        MainWindow w(&api);
        w.show();
        return a.exec();
    }
    
    return 0;
}