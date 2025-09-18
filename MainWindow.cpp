#include "MainWindow.h"
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("OneCT");
    resize(800, 600);
    showLoginInterface();
}

void MainWindow::showLoginInterface()
{
    AuthWidget *authWidget = new AuthWidget(this);
    connect(authWidget, &AuthWidget::loginSuccessful, this, &MainWindow::onLoginSuccessful);
    setCentralWidget(authWidget);
}

void MainWindow::onLoginSuccessful(const QString &token, int userId, const QString &username)
{
    authToken = token;
    currentUserId = userId;
    currentUsername = username;
    showMainInterface();
}

void MainWindow::showMainInterface()
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    QToolBar *toolBar = new QToolBar();
    QAction *feedAction = new QAction("Feed", this);
    QAction *profileAction = new QAction("Profile", this);
    QAction *searchAction = new QAction("Search", this);
    QAction *logoutAction = new QAction("Logout", this);
    
    connect(feedAction, &QAction::triggered, this, &MainWindow::showGlobalFeed);
    connect(profileAction, &QAction::triggered, this, [this]() { showUserProfile(-1); });
    connect(searchAction, &QAction::triggered, this, &MainWindow::showSearch);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onAuthRequired);
    
    toolBar->addAction(feedAction);
    toolBar->addAction(profileAction);
    toolBar->addAction(searchAction);
    toolBar->addAction(logoutAction);
    
    layout->addWidget(toolBar);
    setCentralWidget(centralWidget);
    
    showGlobalFeed();
}

void MainWindow::showGlobalFeed()
{
    WallWidget *wallWidget = new WallWidget(authToken, -1, this);
    connect(wallWidget, &WallWidget::backRequested, this, &MainWindow::showMainInterface);
    connect(wallWidget, &WallWidget::postSelected, this, &MainWindow::showPostComments);
    connect(wallWidget, &WallWidget::userSelected, this, &MainWindow::showUserProfile);
    connect(wallWidget, &WallWidget::authRequired, this, &MainWindow::onAuthRequired);
    setCentralWidget(wallWidget);
}

void MainWindow::showUserProfile(int userId)
{
    ProfileWidget *profileWidget = new ProfileWidget(authToken, userId, this);
    connect(profileWidget, &ProfileWidget::backRequested, this, &MainWindow::showMainInterface);
    connect(profileWidget, &ProfileWidget::messageUserRequested, this, &MainWindow::showPostComments);
    connect(profileWidget, &ProfileWidget::authRequired, this, &MainWindow::onAuthRequired);
    setCentralWidget(profileWidget);
}

void MainWindow::showPostComments(int postId)
{
    CommentsWidget *commentsWidget = new CommentsWidget(authToken, this);
    commentsWidget->loadComments(postId);
    connect(commentsWidget, &CommentsWidget::authRequired, this, &MainWindow::onAuthRequired);
    setCentralWidget(commentsWidget);
}

void MainWindow::showSearch()
{
    SearchWidget *searchWidget = new SearchWidget(authToken, this);
    connect(searchWidget, &SearchWidget::backRequested, this, &MainWindow::showMainInterface);
    connect(searchWidget, &SearchWidget::userSelected, this, &MainWindow::showUserProfile);
    connect(searchWidget, &SearchWidget::authRequired, this, &MainWindow::onAuthRequired);
    setCentralWidget(searchWidget);
}

void MainWindow::onAuthRequired()
{
    authToken.clear();
    currentUserId = -1;
    currentUsername.clear();
    showLoginInterface();
}