#pragma once
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QSettings>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QSettings &settings, QWidget *parent = nullptr);
    bool isDark() const;
private slots:
    void onAccepted();
private:
    QCheckBox *m_dark;
    QSettings &m_settings;
};