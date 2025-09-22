#pragma once
#include <QDialog>

class QLineEdit;
class QPushButton;
class ApiClient;

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(ApiClient* api, QWidget *parent = nullptr);

private slots:
    void tryLogin();

private:
    ApiClient* api;
    QLineEdit* emailEdit;
    QLineEdit* passEdit;
    QPushButton* loginBtn;
};