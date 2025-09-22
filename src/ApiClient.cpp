#include "ApiClient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QJsonDocument>

ApiClient::ApiClient(QObject* parent) : QObject(parent) {
}

QJsonObject ApiClient::sendRequest(const QString& endpoint, const QUrlQuery& params, bool post) {
    QUrl fullUrl(baseUrl + endpoint);
    QNetworkRequest request(fullUrl);
    
    if (post) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    }

    QNetworkReply* reply = nullptr;
    if (post) {
        QByteArray postData = params.toString(QUrl::FullyEncoded).toUtf8();
        reply = manager.post(request, postData);
    } else {
        if (!params.isEmpty()) {
            fullUrl.setQuery(params);
            request.setUrl(fullUrl);
        }
        reply = manager.get(request);
    }

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) return doc.object();
    return QJsonObject();
}

bool ApiClient::login(const QString& email, const QString& password, const QString& code) {
    QUrlQuery params;
    params.addQueryItem("method", "login");
    params.addQueryItem("email", email);
    params.addQueryItem("pass", password);
    if (!code.isEmpty()) {
        params.addQueryItem("code", code);
    }

    QJsonObject result = sendRequest("account.php", params);
    if (result.contains("token")) {
        token = result["token"].toString();
        return true;
    }
    return false;
}

bool ApiClient::checkToken() {
    QUrlQuery params;
    params.addQueryItem("method", "check");
    params.addQueryItem("token", token);

    QJsonObject result = sendRequest("account.php", params);
    return result.contains("valid") && result["valid"].toBool();
}

QJsonObject ApiClient::getProfile() {
    QUrlQuery params;
    params.addQueryItem("method", "profile");
    params.addQueryItem("token", token);

    return sendRequest("user.php", params);
}

QJsonObject ApiClient::getUser(const QString& userIds) {
    QUrlQuery params;
    params.addQueryItem("method", "getuser");
    params.addQueryItem("id", userIds);

    return sendRequest("user.php", params);
}

bool ApiClient::updateProfile(const QString& name, const QString& desc, bool wallOpen) {
    QUrlQuery params;
    params.addQueryItem("method", "change");
    params.addQueryItem("token", token);
    params.addQueryItem("name", name);
    params.addQueryItem("desc", desc);
    params.addQueryItem("wall", wallOpen ? "1" : "0");

    QJsonObject result = sendRequest("user.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

QJsonArray ApiClient::searchUsers(const QString& query, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "get");
    params.addQueryItem("q", query);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("search.php", params);
    if (result.contains("users") && result["users"].isArray()) {
        return result["users"].toArray();
    }
    return QJsonArray();
}

QJsonArray ApiClient::getGlobalWall(int page) {
    QUrlQuery params;
    params.addQueryItem("method", "getglobal");
    params.addQueryItem("token", token);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("wall.php", params);
    if (result.contains("posts") && result["posts"].isArray()) {
        return result["posts"].toArray();
    }
    return QJsonArray();
}

QJsonArray ApiClient::getUserWall(const QString& userId, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "getbyuser");
    params.addQueryItem("token", token);
    params.addQueryItem("id", userId);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("wall.php", params);
    if (result.contains("posts") && result["posts"].isArray()) {
        return result["posts"].toArray();
    }
    return QJsonArray();
}

bool ApiClient::deletePost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "delete");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

bool ApiClient::likePost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "like");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

bool ApiClient::pinPost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "pin");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

bool ApiClient::addPost(const QString& targetUserId, const QString& text) {
    QUrlQuery params;
    params.addQueryItem("method", "add");
    params.addQueryItem("token", token);
    params.addQueryItem("id", targetUserId);
    params.addQueryItem("text", text);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

QJsonArray ApiClient::getComments(const QString& postId, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "get");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);
    params.addQueryItem("page", QString::number(page));

    QJsonObject result = sendRequest("comments.php", params);
    if (result.contains("comments") && result["comments"].isArray()) {
        return result["comments"].toArray();
    }
    return QJsonArray();
}

bool ApiClient::deleteComment(const QString& commentId) {
    QUrlQuery params;
    params.addQueryItem("method", "delete");
    params.addQueryItem("token", token);
    params.addQueryItem("id", commentId);

    QJsonObject result = sendRequest("comments.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}

bool ApiClient::addComment(const QString& postId, const QString& text) {
    QUrlQuery params;
    params.addQueryItem("method", "add");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);
    params.addQueryItem("text", text);

    QJsonObject result = sendRequest("comments.php", params, true);
    return result.contains("success") && result["success"].toInt() == 1;
}