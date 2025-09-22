#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

SettingsDialog::SettingsDialog(QSettings &settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    m_dark = new QCheckBox("Dark theme");
    m_dark->setChecked(m_settings.value("dark", false).toBool());
    QPushButton *ok = new QPushButton("OK");
    QPushButton *cancel = new QPushButton("Cancel");
    QHBoxLayout *h = new QHBoxLayout; h->addWidget(ok); h->addWidget(cancel);
    QVBoxLayout *v = new QVBoxLayout; v->addWidget(m_dark); v->addLayout(h);
    setLayout(v);
    connect(ok, &QPushButton::clicked, this, &SettingsDialog::onAccepted);
    connect(cancel, &QPushButton::clicked, this, &SettingsDialog::reject);
}

bool SettingsDialog::isDark() const { return m_dark->isChecked(); }

void SettingsDialog::onAccepted() { m_settings.setValue("dark", m_dark->isChecked()); accept(); }