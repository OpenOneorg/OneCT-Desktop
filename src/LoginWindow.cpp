#include "LoginWindow.h"
#include "ApiClient.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>

LoginWindow::LoginWindow(ApiClient* api, QWidget *parent)
    : QDialog(parent), api(api) {
    setWindowTitle("Вход");
    setFixedSize(300, 200);

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("example@mail.com");
    passEdit = new QLineEdit(this);
    passEdit->setEchoMode(QLineEdit::Password);
    loginBtn = new QPushButton("Войти", this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Почта:"));
    layout->addWidget(emailEdit);
    layout->addWidget(new QLabel("Пароль:"));
    layout->addWidget(passEdit);
    layout->addWidget(loginBtn);
    setLayout(layout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::tryLogin);
}

void LoginWindow::tryLogin() {
    QString email = emailEdit->text();
    QString pass = passEdit->text();

    if (email.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите почту и пароль");
        return;
    }

    if (api->login(email, pass, "")) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Неверная почта или пароль");
    }
}