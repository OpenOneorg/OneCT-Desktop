#include "MainWindow.h"
#include "ApiClient.h"
#include "PostWidget.h"
#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QCheckBox>

MainWindow::MainWindow(ApiClient* api, QWidget *parent)
    : QMainWindow(parent), api(api) {
    resize(800, 600);
    setWindowTitle("OneCT Messenger");

    QToolBar* toolbar = addToolBar("Main");
    QAction* homeAct = toolbar->addAction("Глобальная лента");
    QAction* profileAct = toolbar->addAction("Профиль");
    QAction* searchAct = toolbar->addAction("Поиск");
    QAction* settingsAct = toolbar->addAction("Настройки");

    tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    QWidget* wallTab = new QWidget;
    wallLayout = new QVBoxLayout;
    wallTab->setLayout(wallLayout);
    tabs->addTab(wallTab, "Лента");

    QWidget* profileTab = new QWidget;
    profileLayout = new QVBoxLayout;
    profileTab->setLayout(profileLayout);
    tabs->addTab(profileTab, "Профиль");

    QWidget* searchTab = new QWidget;
    QVBoxLayout* searchLayout = new QVBoxLayout;
    searchTab->setLayout(searchLayout);
    tabs->addTab(searchTab, "Поиск");

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Введите имя для поиска");
    searchBtn = new QPushButton("Найти");
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);

    connect(homeAct, &QAction::triggered, this, &MainWindow::loadGlobalWall);
    connect(profileAct, &QAction::triggered, this, &MainWindow::loadProfile);
    connect(searchAct, &QAction::triggered, this, &MainWindow::searchUsers);
    connect(settingsAct, &QAction::triggered, this, &MainWindow::openSettings);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::searchUsers);

    loadGlobalWall();
    loadProfile();
}

void MainWindow::loadProfile() {
    QLayoutItem* item;
    while ((item = profileLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QJsonObject prof = api->getProfile();
    
    nameEdit = new QLineEdit(prof["name"].toString());
    descEdit = new QLineEdit(prof["desc"].toString());
    wallOpenCheck = new QCheckBox("Стена открыта");
    wallOpenCheck->setChecked(prof["wall_open"].toBool());
    saveProfileBtn = new QPushButton("Сохранить");

    profileLayout->addWidget(new QLabel("Имя:"));
    profileLayout->addWidget(nameEdit);
    profileLayout->addWidget(new QLabel("Описание:"));
    profileLayout->addWidget(descEdit);
    profileLayout->addWidget(wallOpenCheck);
    profileLayout->addWidget(saveProfileBtn);

    connect(saveProfileBtn, &QPushButton::clicked, this, &MainWindow::editProfile);
}

void MainWindow::loadGlobalWall() {
    QLayoutItem* item;
    while ((item = wallLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QJsonArray posts = api->getGlobalWall();
    for (auto p : posts) {
        wallLayout->addWidget(new PostWidget(p.toObject(), api));
    }

    postEdit = new QLineEdit;
    postEdit->setPlaceholderText("Написать что-нибудь на свою стену...");
    postBtn = new QPushButton("Опубликовать");

    QWidget* postBox = new QWidget;
    QHBoxLayout* postLayout = new QHBoxLayout;
    postLayout->addWidget(postEdit);
    postLayout->addWidget(postBtn);
    postBox->setLayout(postLayout);

    wallLayout->addWidget(postBox);
    wallLayout->addStretch();

    connect(postBtn, &QPushButton::clicked, this, &MainWindow::addNewPost);
}

void MainWindow::addNewPost() {
    QString text = postEdit->text();
    if (text.isEmpty()) return;

    if (api->addPost("0", text)) {
        postEdit->clear();
        loadGlobalWall();
        QMessageBox::information(this, "Успех", "Пост опубликован");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось опубликовать пост");
    }
}

void MainWindow::searchUsers() {
    QString query = searchEdit->text();
    if (query.isEmpty()) return;

    QJsonArray users = api->searchUsers(query);
    
    QWidget* searchTab = tabs->widget(2);
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(searchTab->layout());
    
    for (int i = layout->count() - 1; i >= 2; i--) {
        QLayoutItem* item = layout->takeAt(i);
        delete item->widget();
        delete item;
    }

    for (auto user : users) {
        QJsonObject userObj = user.toObject();
        QLabel* userLabel = new QLabel(userObj["name"].toString() + " - " + userObj["email"].toString());
        layout->addWidget(userLabel);
    }
}

void MainWindow::editProfile() {
    QString name = nameEdit->text();
    QString desc = descEdit->text();
    bool wallOpen = wallOpenCheck->isChecked();

    if (api->updateProfile(name, desc, wallOpen)) {
        QMessageBox::information(this, "Успех", "Профиль обновлен");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось обновить профиль");
    }
}

void MainWindow::openSettings() {
    QSettings settings("OneCT", "Desktop");
    SettingsDialog dlg(settings, this);
    dlg.exec();
}
//заебусь писать