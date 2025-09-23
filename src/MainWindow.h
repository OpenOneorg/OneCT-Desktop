#pragma once
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QCheckBox>
#include <QLabel>
#include <QDateTime>

class ApiClient;
class PostWidget;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ApiClient* api, QWidget *parent = nullptr);

private slots:
    void onNavigationClicked(QListWidgetItem* item);
    void loadGlobalWall();
    void loadMorePosts();
    void loadProfile();
    void searchUsers();
    void editProfile();
    void addNewPost();

private:
    void setupUI();
    void loadUserAvatar(const QString& url, QLabel* avatarLabel);

    ApiClient* api;
    
    QWidget* centralWidget;
    QHBoxLayout* mainLayout;
    QListWidget* navList;
    QStackedWidget* contentStack;
    
    QWidget* wallPage;
    QWidget* searchPage;
    QWidget* profilePage;
    QWidget* settingsPage;
    
    QWidget* wallContent;
    QVBoxLayout* wallLayout;
    QVBoxLayout* profileLayout;
    QVBoxLayout* searchLayout;
    
    QLineEdit* searchEdit;
    QPushButton* searchBtn;
    QLineEdit* nameEdit;
    QLineEdit* descEdit;
    QCheckBox* wallOpenCheck;
    QPushButton* saveProfileBtn;
    QLineEdit* postEdit;
    QPushButton* postBtn;
    QPushButton* loadMoreBtn;
    
    QLabel* profileAvatar;
    QLabel* profileName;
    QLabel* profileDesc;
    
    int currentWallPage;
};