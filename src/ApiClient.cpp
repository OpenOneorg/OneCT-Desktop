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

//

bool ApiClient::checkToken() {
    QUrlQuery params;
    params.addQueryItem("method", "check");
    params.addQueryItem("token", token);

    QJsonObject result = sendRequest("account.php", params);
    qDebug() << "Token check result:" << result;
    
    if (result.contains("valid")) {
        return result["valid"].toBool();
    }
    if (result.contains("status")) {
        return result["status"].toString() == "valid";
    }
    if (result.contains("success")) {
        return result["success"].toBool();
    }
    
    return false;
}

QJsonObject ApiClient::getProfile() {
    QUrlQuery params;
    params.addQueryItem("method", "profile");
    params.addQueryItem("token", token);

    QJsonObject result = sendRequest("user.php", params);
    qDebug() << "Profile response:" << result;
    return result;
}

bool ApiClient::updateProfile(const QString& name, const QString& desc, bool wallOpen) {
    QUrlQuery params;
    params.addQueryItem("method", "change");
    params.addQueryItem("token", token);
    params.addQueryItem("name", name);
    params.addQueryItem("desc", desc);
    params.addQueryItem("wall", wallOpen ? "1" : "0");

    QJsonObject result = sendRequest("user.php", params, true);
    qDebug() << "Update profile result:" << result;
    
    if (result.contains("success") && result["success"].toInt() == 1) {
        return true;
    }
    if (result.contains("status") && result["status"].toString() == "success") {
        return true;
    }
    if (result.contains("result") && result["result"].toInt() == 1) {
        return true;
    }
    
    return false;
}

QJsonArray ApiClient::getGlobalWall(int page) {
    QUrlQuery params;
    params.addQueryItem("method", "getglobal");
    params.addQueryItem("token", token);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("wall.php", params);
    qDebug() << "Global wall page" << page << "response:" << result;

    if (result.contains("posts") && result["posts"].isArray()) {
        return result["posts"].toArray();
    }
    if (result.contains("data") && result["data"].isArray()) {
        return result["data"].toArray();
    }
    if (result.contains("wall") && result["wall"].isArray()) {
        return result["wall"].toArray();
    }
    if (result.contains("messages") && result["messages"].isArray()) {
        return result["messages"].toArray();
    }
    
    return QJsonArray();
}

bool ApiClient::addPost(const QString& targetUserId, const QString& text) {
    QUrlQuery params;
    params.addQueryItem("method", "add");
    params.addQueryItem("token", token);
    params.addQueryItem("text", text);
    
    if (!targetUserId.isEmpty() && targetUserId != "0") {
        params.addQueryItem("id", targetUserId);
    }

    QJsonObject result = sendRequest("wall.php", params, true);
    qDebug() << "Add post result with ID" << targetUserId << ":" << result;
    
    if (result.contains("error")) {
        qDebug() << "API Error:" << result["error"].toString();
        return false;
    }
    
    return true;
}

//


QJsonObject ApiClient::getUser(const QString& userIds) {
    QUrlQuery params;
    params.addQueryItem("method", "getuser");
    params.addQueryItem("id", userIds);

    return sendRequest("user.php", params);
}

QJsonArray ApiClient::searchUsers(const QString& query, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "get");
    params.addQueryItem("q", query);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("search.php", params);
    return result.value("users").toArray();
}

QJsonArray ApiClient::getUserWall(const QString& userId, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "getbyuser");
    params.addQueryItem("token", token);
    params.addQueryItem("id", userId);
    params.addQueryItem("p", QString::number(page));

    QJsonObject result = sendRequest("wall.php", params);
    return result.value("posts").toArray();
}

bool ApiClient::deletePost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "delete");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.value("success").toInt() == 1;
}

bool ApiClient::likePost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "like");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.value("liked").toInt() == 1;
}

bool ApiClient::pinPost(const QString& postId) {
    QUrlQuery params;
    params.addQueryItem("method", "pin");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);

    QJsonObject result = sendRequest("wall.php", params, true);
    return result.value("pinned").toInt() == 1;
}


QJsonArray ApiClient::getComments(const QString& postId, int page) {
    QUrlQuery params;
    params.addQueryItem("method", "get");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);
    params.addQueryItem("page", QString::number(page));

    QJsonObject result = sendRequest("comments.php", params);
    return result.value("comments").toArray();
}

bool ApiClient::deleteComment(const QString& commentId) {
    QUrlQuery params;
    params.addQueryItem("method", "delete");
    params.addQueryItem("token", token);
    params.addQueryItem("id", commentId);

    QJsonObject result = sendRequest("comments.php", params, true);
    return result.value("success").toInt() == 1;
}

bool ApiClient::addComment(const QString& postId, const QString& text) {
    QUrlQuery params;
    params.addQueryItem("method", "add");
    params.addQueryItem("token", token);
    params.addQueryItem("id", postId);
    params.addQueryItem("text", text);

    QJsonObject result = sendRequest("comments.php", params, true);
    return result.value("success").toInt() == 1;
}