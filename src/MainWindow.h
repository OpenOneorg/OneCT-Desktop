#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QCheckBox>

class ApiClient;
class PostWidget;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ApiClient* api, QWidget *parent = nullptr);

private slots:
    void loadProfile();
    void loadGlobalWall();
    void openSettings();
    void searchUsers();
    void editProfile();
    void addNewPost();

private:
    ApiClient* api;
    QTabWidget* tabs;
    QVBoxLayout* profileLayout;
    QVBoxLayout* wallLayout;
    QLineEdit* searchEdit;
    QPushButton* searchBtn;
    QLineEdit* nameEdit;
    QLineEdit* descEdit;
    QCheckBox* wallOpenCheck;
    QPushButton* saveProfileBtn;
    QLineEdit* postEdit;
    QPushButton* postBtn;
};