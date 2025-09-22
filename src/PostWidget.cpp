#include "PostWidget.h"
#include "ApiClient.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

PostWidget::PostWidget(const QJsonObject &post, ApiClient* api, QWidget *parent)
    : QWidget(parent), api(api) {
    
    postId = post.value("id").toString();
    QString text = post.value("text").toString();
    QString author = post.value("author").toString();
    int likes = post.value("likes").toInt();
    bool isLiked = post.value("is_liked").toBool();

    QLabel *authorLabel = new QLabel(author);
    QLabel *textLabel = new QLabel(text);
    likeBtn = new QPushButton(isLiked ? "❤️" : "🤍");
    likeCountLabel = new QLabel(QString::number(likes));
    QPushButton *commentBtn = new QPushButton("Комментарии");
    QPushButton *deleteBtn = new QPushButton("Удалить");

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(likeBtn);
    buttonsLayout->addWidget(likeCountLabel);
    buttonsLayout->addWidget(commentBtn);
    buttonsLayout->addWidget(deleteBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(authorLabel);
    mainLayout->addWidget(textLabel);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    connect(likeBtn, &QPushButton::clicked, this, &PostWidget::onLikeClicked);
    connect(commentBtn, &QPushButton::clicked, this, &PostWidget::onCommentClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &PostWidget::onDeleteClicked);
}

void PostWidget::onLikeClicked() {
    if (api->likePost(postId)) {
    }
}

void PostWidget::onCommentClicked() {

}

void PostWidget::onDeleteClicked() {
    if (api->deletePost(postId)) {
        deleteLater();
    }
}