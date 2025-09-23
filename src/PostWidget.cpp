#include "PostWidget.h"
#include "ApiClient.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QNetworkReply>
#include <QPainter>
#include <QPainterPath>
#include <QDateTime>

PostWidget::PostWidget(const QJsonObject &post, ApiClient* api, QWidget *parent)
    : QWidget(parent), api(api) {
    
    postId = post.value("id").toString();
    if (postId.isEmpty()) postId = post.value("post_id").toString();
    
    QString text = post.value("text").toString();
    if (text.isEmpty()) text = post.value("message").toString();
    if (text.isEmpty()) text = post.value("content").toString();
    
    QString authorName = post.value("author").toString();
    if (authorName.isEmpty()) authorName = post.value("username").toString();
    if (authorName.isEmpty()) authorName = post.value("name").toString();
    if (authorName.isEmpty()) authorName = "Неизвестный";
    
    QString authorAvatar = post.value("avatar").toString();
    if (authorAvatar.isEmpty()) authorAvatar = post.value("img").toString();
    if (authorAvatar.isEmpty()) authorAvatar = post.value("author_avatar").toString();
    
    QString dateStr = post.value("date").toString();
    if (dateStr.isEmpty()) dateStr = post.value("timestamp").toString();
    if (dateStr.isEmpty()) dateStr = post.value("time").toString();
    
    QDateTime postDate;
    if (!dateStr.isEmpty()) {
        postDate = QDateTime::fromString(dateStr, Qt::ISODate);
        if (!postDate.isValid()) {
            postDate = QDateTime::fromSecsSinceEpoch(dateStr.toLongLong());
        }
    }
    
    if (!postDate.isValid()) {
        postDate = QDateTime::currentDateTime();
    }
    
    QString formattedDate = postDate.toString("dd.MM.yyyy HH:mm");
    
    int likes = post.value("likes").toInt();
    if (likes == 0) likes = post.value("likes_count").toInt();
    
    bool isLiked = post.value("is_liked").toBool();
    if (!post.value("liked").isUndefined()) {
        isLiked = post.value("liked").toBool();
    }
    
    QLabel* avatarLabel = new QLabel();
    avatarLabel->setFixedSize(50, 50);
    avatarLabel->setStyleSheet("border: 1px solid gray; border-radius: 25px; background-color: #f0f0f0;");
    
    if (!authorAvatar.isEmpty()) {
        QUrl imageUrl(authorAvatar);
        QNetworkRequest request(imageUrl);
        QNetworkReply* reply = api->getNetworkManager()->get(request);
        
        connect(reply, &QNetworkReply::finished, this, [reply, avatarLabel]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                QPixmap pixmap;
                if (pixmap.loadFromData(data)) {
                    QPixmap rounded = pixmap.scaled(50, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                    
                    QPixmap circularPixmap(50, 50);
                    circularPixmap.fill(Qt::transparent);
                    
                    QPainter painter(&circularPixmap);
                    painter.setRenderHint(QPainter::Antialiasing);
                    QPainterPath path;
                    path.addEllipse(0, 0, 50, 50);
                    painter.setClipPath(path);
                    painter.drawPixmap(0, 0, rounded);
                    
                    avatarLabel->setPixmap(circularPixmap);
                }
            }
            reply->deleteLater();
        });
    } else {
        QPixmap placeholder(50, 50);
        placeholder.fill(QColor(200, 200, 200));
        
        QPainter painter(&placeholder);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(5, 5, 40, 40);
        painter.setPen(Qt::black);
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "👤");
        
        avatarLabel->setPixmap(placeholder);
    }
    
    QLabel* authorLabel = new QLabel(authorName);
    authorLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    QLabel* dateLabel = new QLabel(formattedDate);
    dateLabel->setStyleSheet("color: #666; font-size: 12px;");
    
    QLabel* textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("margin: 5px 0px;");
    
    likeBtn = new QPushButton(isLiked ? "❤️" : "🤍");
    likeBtn->setFixedSize(30, 30);
    likeBtn->setStyleSheet("border: none; background: transparent;");
    
    likeCountLabel = new QLabel(QString::number(likes));
    likeCountLabel->setStyleSheet("color: #666; font-size: 12px;");
    
    QPushButton* commentBtn = new QPushButton("💬");
    commentBtn->setFixedSize(30, 30);
    commentBtn->setStyleSheet("border: none; background: transparent;");
    
    QPushButton* deleteBtn = new QPushButton("🗑️");
    deleteBtn->setFixedSize(30, 30);
    deleteBtn->setStyleSheet("border: none; background: transparent;");

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(avatarLabel);
    
    QVBoxLayout* authorLayout = new QVBoxLayout();
    authorLayout->addWidget(authorLabel);
    authorLayout->addWidget(dateLabel);
    headerLayout->addLayout(authorLayout);
    headerLayout->addStretch();
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(likeBtn);
    buttonsLayout->addWidget(likeCountLabel);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(commentBtn);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(deleteBtn);
    buttonsLayout->addStretch();
    
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(textLabel);
    mainLayout->addLayout(buttonsLayout);
    
    setLayout(mainLayout);
    setStyleSheet("border: 1px solid #ddd; border-radius: 10px; padding: 15px; margin: 10px; background-color: white;");
    
    connect(likeBtn, &QPushButton::clicked, this, &PostWidget::onLikeClicked);
    connect(commentBtn, &QPushButton::clicked, this, &PostWidget::onCommentClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &PostWidget::onDeleteClicked);
}

void PostWidget::onLikeClicked() {
    if (api->likePost(postId)) {
        bool currentLiked = likeBtn->text() == "❤️";
        likeBtn->setText(currentLiked ? "🤍" : "❤️");
        
        int currentLikes = likeCountLabel->text().toInt();
        likeCountLabel->setText(QString::number(currentLiked ? currentLikes - 1 : currentLikes + 1));
    }
}

void PostWidget::onCommentClicked() {
    qDebug() << "Comments for post" << postId;
}

void PostWidget::onDeleteClicked() {
    if (api->deletePost(postId)) {
        deleteLater();
    }
}