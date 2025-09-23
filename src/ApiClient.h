#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QString>
#include <QDebug>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject* parent = nullptr);

    bool login(const QString& email, const QString& password, const QString& code = "");
    bool checkToken();
    QJsonObject getProfile();
    QJsonObject getUser(const QString& userIds);
    bool updateProfile(const QString& name, const QString& desc, bool wallOpen);
    QJsonArray searchUsers(const QString& query, int page = 1);
    QJsonArray getGlobalWall(int page = 1);
    QJsonArray getUserWall(const QString& userId, int page = 1);
    bool deletePost(const QString& postId);
    bool likePost(const QString& postId);
    bool pinPost(const QString& postId);
    bool addPost(const QString& targetUserId, const QString& text);
    QJsonArray getComments(const QString& postId, int page = 1);
    bool deleteComment(const QString& commentId);
    bool addComment(const QString& postId, const QString& text);

    QString getToken() const { return token; }
    QNetworkAccessManager* getNetworkManager() { return &manager; }

private:
    QString token;
    QString baseUrl = "https://onect.h1n.ru/api/";
    QNetworkAccessManager manager;
    QJsonObject sendRequest(const QString& endpoint, const QUrlQuery& params = QUrlQuery(), bool post = false);
};