#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getSelectedRole() const;
    QString getUsername() const;

private slots:
    void on_btnRoleVigilance_clicked();
    void on_btnRoleProtection_clicked();
    void on_btnRoleDecideur_clicked();
    void on_btnLogin_clicked();

private:
    Ui::LoginDialog *ui;
    QString currentRole;
};

#endif // LOGINDIALOG_H
