#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AuthWidget.h"
#include "WallWidget.h"
#include "ProfileWidget.h"
#include "CommentsWidget.h"
#include "SearchWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void onLoginSuccessful(const QString &token, int userId, const QString &username);
    void showGlobalFeed();
    void showUserProfile(int userId = -1);
    void showPostComments(int postId);
    void showSearch();
    void onAuthRequired();
private:
    QString authToken;
    int currentUserId;
    QString currentUsername;
    void showLoginInterface();
    void showMainInterface();
};

#endif
