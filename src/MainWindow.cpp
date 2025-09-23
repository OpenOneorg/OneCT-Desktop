#include "MainWindow.h"
#include "ApiClient.h"
#include "PostWidget.h"
#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QCheckBox>
#include <QScrollArea>
#include <QNetworkReply>
#include <QPainter>
#include <QPainterPath>
#include <QDateTime>
#include <algorithm>
#include <QApplication>

MainWindow::MainWindow(ApiClient* api, QWidget *parent)
    : QMainWindow(parent), api(api), currentWallPage(1) {
    setupUI();
    loadGlobalWall();
    loadProfile();
}

void MainWindow::setupUI() {
    setWindowTitle("OneCT Messenger");
    setMinimumSize(1000, 600);
    
    centralWidget = new QWidget(this);
    mainLayout = new QHBoxLayout(centralWidget);
    
    navList = new QListWidget();
    navList->setFixedWidth(150);
    navList->addItem("📋 Лента");
    navList->addItem("🔍 Поиск");
    navList->addItem("👤 Профиль");
    navList->addItem("⚙️ Настройки");
    
    contentStack = new QStackedWidget();
    
    mainLayout->addWidget(navList);
    mainLayout->addWidget(contentStack, 1);
    
    setCentralWidget(centralWidget);
    
    connect(navList, &QListWidget::itemClicked, this, &MainWindow::onNavigationClicked);
    
    wallPage = new QWidget();
    QVBoxLayout* wallPageLayout = new QVBoxLayout(wallPage);
    
    QWidget* postPanel = new QWidget();
    postPanel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-bottom: 1px solid #ccc;");
    QVBoxLayout* postPanelLayout = new QVBoxLayout(postPanel);
    
    QLabel* postLabel = new QLabel("OneCT");
    postLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    
    postEdit = new QLineEdit();
    postEdit->setPlaceholderText("Что у вас нового?");
    postEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");
    
    postBtn = new QPushButton("Опубликовать");
    postBtn->setStyleSheet("padding: 8px; background-color: #007bff; color: white; border: none; border-radius: 4px;");
    
    QHBoxLayout* postLayout = new QHBoxLayout();
    postLayout->addWidget(postLabel);
    postLayout->addStretch();
    
    QHBoxLayout* postInputLayout = new QHBoxLayout();
    postInputLayout->addWidget(postEdit, 1);
    postInputLayout->addWidget(postBtn);
    
    postPanelLayout->addLayout(postLayout);
    postPanelLayout->addLayout(postInputLayout);

    loadMoreBtn = new QPushButton("Загрузить предыдущие сообщения");
    loadMoreBtn->setStyleSheet("padding: 8px; margin: 10px;");
    loadMoreBtn->setVisible(false);

    QScrollArea* wallScroll = new QScrollArea();
    wallScroll->setWidgetResizable(true);
    wallContent = new QWidget();
    wallLayout = new QVBoxLayout(wallContent);
    wallScroll->setWidget(wallContent);
    
    wallPageLayout->addWidget(postPanel);
    wallPageLayout->addWidget(loadMoreBtn);
    wallPageLayout->addWidget(wallScroll, 1);
    
    connect(postBtn, &QPushButton::clicked, this, &MainWindow::addNewPost);
    connect(loadMoreBtn, &QPushButton::clicked, this, &MainWindow::loadMorePosts);

    searchPage = new QWidget();
    searchLayout = new QVBoxLayout(searchPage);
    
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Введите имя пользователя...");
    searchBtn = new QPushButton("Найти");
    
    QHBoxLayout* searchHeaderLayout = new QHBoxLayout();
    searchHeaderLayout->addWidget(searchEdit);
    searchHeaderLayout->addWidget(searchBtn);
    
    searchLayout->addLayout(searchHeaderLayout);
    
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::searchUsers);

    profilePage = new QWidget();
    profileLayout = new QVBoxLayout(profilePage);
    
    profileAvatar = new QLabel();
    profileAvatar->setFixedSize(100, 100);
    profileAvatar->setStyleSheet("border: 2px solid gray; border-radius: 50px;");
    profileAvatar->setAlignment(Qt::AlignCenter);
    profileAvatar->setText("🖼️");
    
    profileName = new QLabel();
    profileName->setStyleSheet("font-size: 18px; font-weight: bold;");
    
    profileDesc = new QLabel();
    profileDesc->setWordWrap(true);
    
    nameEdit = new QLineEdit();
    descEdit = new QLineEdit();
    wallOpenCheck = new QCheckBox("Открытая стена");
    saveProfileBtn = new QPushButton("Сохранить профиль");
    
    profileLayout->addWidget(profileAvatar, 0, Qt::AlignCenter);
    profileLayout->addWidget(profileName, 0, Qt::AlignCenter);
    profileLayout->addWidget(profileDesc);
    profileLayout->addWidget(new QLabel("Имя:"));
    profileLayout->addWidget(nameEdit);
    profileLayout->addWidget(new QLabel("Описание:"));
    profileLayout->addWidget(descEdit);
    profileLayout->addWidget(wallOpenCheck);
    profileLayout->addWidget(saveProfileBtn);
    
    connect(saveProfileBtn, &QPushButton::clicked, this, &MainWindow::editProfile);

    settingsPage = new QWidget();
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsPage);
    QPushButton* settingsBtn = new QPushButton("Открыть настройки");
    settingsLayout->addWidget(settingsBtn);
    
    connect(settingsBtn, &QPushButton::clicked, this, [this]() {
        QSettings settings("OneCT", "Messenger");
        SettingsDialog dlg(settings, this);
        dlg.exec();
    });
    
    contentStack->addWidget(wallPage);
    contentStack->addWidget(searchPage);
    contentStack->addWidget(profilePage);
    contentStack->addWidget(settingsPage);
}

void MainWindow::onNavigationClicked(QListWidgetItem* item) {
    int row = navList->row(item);
    contentStack->setCurrentIndex(row);
    
    switch(row) {
        case 0: loadGlobalWall(); break;
        case 1: break;
        case 2: loadProfile(); break;
    }
}

void MainWindow::loadGlobalWall() {
    currentWallPage = 1;
    
    QLayoutItem* item;
    while ((item = wallLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    QJsonArray posts = api->getGlobalWall(currentWallPage);
    
    if (posts.isEmpty()) {
        QLabel* emptyLabel = new QLabel("Лента пуста\nБудьте первым, кто напишет сообщение!");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #666; padding: 40px; font-size: 14px;");
        wallLayout->addWidget(emptyLabel);
    } else {

        QList<QJsonObject> postList;
        for (const auto& postValue : posts) {
            postList.append(postValue.toObject());
        }
        

        std::sort(postList.begin(), postList.end(), [](const QJsonObject& a, const QJsonObject& b) {
            qint64 timeA = a.value("timestamp").toVariant().toLongLong();
            qint64 timeB = b.value("timestamp").toVariant().toLongLong();
            return timeA > timeB;
        });
        
        for (const auto& post : postList) {
            PostWidget* postWidget = new PostWidget(post, api);
            wallLayout->addWidget(postWidget);
        }
    }
    
    loadMoreBtn->setVisible(posts.size() >= 10);
    wallLayout->addStretch();
}

void MainWindow::loadMorePosts() {
    currentWallPage++;
    
    QJsonArray posts = api->getGlobalWall(currentWallPage);
    
    if (!posts.isEmpty()) {

        for (int i = posts.size() - 1; i >= 0; i--) {
            QJsonObject post = posts[i].toObject();
            PostWidget* postWidget = new PostWidget(post, api);
            wallLayout->insertWidget(0, postWidget);
        }

        loadMoreBtn->setVisible(posts.size() >= 10);
    } else {
        loadMoreBtn->setVisible(false);
        QMessageBox::information(this, "Лента", "Все сообщения загружены");
    }
}

void MainWindow::loadProfile() {
    QJsonObject profile = api->getProfile();
    
    qDebug() << "Profile data:" << profile;
    QStringList keys = profile.keys();
    qDebug() << "Available profile keys:" << keys;
    
    QString username = profile.value("username").toString();
    QString description = profile.value("description").toString();
    
    QString idStr;
    if (profile.contains("id")) {
        if (profile["id"].isString()) {
            idStr = profile["id"].toString();
        } else if (profile["id"].isDouble()) {
            idStr = QString::number(profile["id"].toInt());
        }
    }
    
    QString email = profile.value("email").toString();
    QString avatarUrl = profile.value("img").toString();
    QString avatarUrl100 = profile.value("img100").toString();
    bool wallOpen = profile.value("openwall").toBool();
    
    profileName->setText(username.isEmpty() ? "Не указано" : username);
    profileDesc->setText(description.isEmpty() ? "Описание не указано" : description);
    nameEdit->setText(username);
    descEdit->setText(description);
    wallOpenCheck->setChecked(wallOpen);
    
    QString avatarToLoad = avatarUrl100.isEmpty() ? avatarUrl : avatarUrl100;
    if (!avatarToLoad.isEmpty()) {
        loadUserAvatar(avatarToLoad, profileAvatar);
    } else {
        profileAvatar->setText("👤");
    }
    
    qDebug() << "User ID:" << idStr << "Type:" << (profile.contains("id") ? profile["id"].type() : -1) << "Username:" << username;
}

void MainWindow::searchUsers() {
    QString query = searchEdit->text();
    if (query.isEmpty()) return;
    
    QLayoutItem* item;
    while ((item = searchLayout->takeAt(1)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    QJsonArray users = api->searchUsers(query);
    for (const auto& userValue : users) {
        QJsonObject user = userValue.toObject();
        QLabel* userLabel = new QLabel(
            user.value("name").toString() + " (" + user.value("email").toString() + ")"
        );
        searchLayout->addWidget(userLabel);
    }
}

void MainWindow::editProfile() {
    QString name = nameEdit->text();
    QString desc = descEdit->text();
    bool wallOpen = wallOpenCheck->isChecked();
    
    if (api->updateProfile(name, desc, wallOpen)) {
        QMessageBox::information(this, "Успех", "Профиль обновлен");
        loadProfile();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось обновить профиль");
    }
}

void MainWindow::addNewPost() {
    QString text = postEdit->text().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст поста");
        return;
    }
    
    static bool isPosting = false;
    if (isPosting) {
        return;
    }
    isPosting = true;
    
    postBtn->setEnabled(false);
    postBtn->setText("Отправка...");
    QApplication::processEvents();
    
    QJsonObject profile = api->getProfile();
    QString myId = QString::number(profile["id"].toInt());
    
    qDebug() << "Posting as user ID:" << myId;
    
    static QDateTime lastPostTime = QDateTime::fromSecsSinceEpoch(0);
    QDateTime currentTime = QDateTime::currentDateTime();
    
    if (lastPostTime.secsTo(currentTime) < 30) {
        QMessageBox::warning(this, "Анти-спам", "Пожалуйста, подождите 30 секунд перед отправкой следующего поста");
        postBtn->setEnabled(true);
        postBtn->setText("Опубликовать");
        isPosting = false;
        return;
    }
    
    bool success = api->addPost(myId, text);
    
    if (success) {
        lastPostTime = currentTime;
        postEdit->clear();
        
        loadGlobalWall();
        
        qDebug() << "Post published successfully";
    } else {
        QMessageBox::warning(this, "Ошибка", 
            "Не удалось опубликовать пост.\n"
            "Попробуйте позже.");
    }
    
    postBtn->setEnabled(true);
    postBtn->setText("Опубликовать");
    isPosting = false;
}

void MainWindow::loadUserAvatar(const QString& url, QLabel* avatarLabel) {
    if (url.isEmpty()) {
        avatarLabel->setText("👤");
        return;
    }
    
    QUrl imageUrl(url);
    if (!imageUrl.isValid()) {
        qDebug() << "Invalid avatar URL:" << url;
        avatarLabel->setText("👤");
        return;
    }
    
    QNetworkRequest request(imageUrl);
    QNetworkReply* reply = api->getNetworkManager()->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [reply, avatarLabel]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QPixmap pixmap;
            if (pixmap.loadFromData(data)) {
                QPixmap circularPixmap(avatarLabel->width(), avatarLabel->height());
                circularPixmap.fill(Qt::transparent);
                
                QPainter painter(&circularPixmap);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setBrush(QBrush(pixmap.scaled(avatarLabel->width(), avatarLabel->height(), 
                                                    Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(0, 0, avatarLabel->width(), avatarLabel->height());
                
                avatarLabel->setPixmap(circularPixmap);
            } else {
                qDebug() << "Failed to load avatar image from data";
                avatarLabel->setText("👤");
            }
        } else {
            qDebug() << "Avatar loading error:" << reply->errorString();
            avatarLabel->setText("👤");
        }
        reply->deleteLater();
    });
}