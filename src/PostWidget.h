#pragma once
#include <QWidget>
#include <QJsonObject>

class ApiClient;
class QLabel;
class QPushButton;

class PostWidget : public QWidget {
    Q_OBJECT
public:
    explicit PostWidget(const QJsonObject &post, ApiClient* api, QWidget *parent = nullptr);

//чето нажимается типо
private slots:
    void onLikeClicked();
    void onCommentClicked();
    void onDeleteClicked();

private:
    QString postId;
    ApiClient* api;
    QLabel* likeCountLabel;
    QPushButton* likeBtn;
};