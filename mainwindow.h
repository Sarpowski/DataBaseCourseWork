#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString  hashPassword(const QString &plainPassword) {
        return QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    }
    void navigateToRoleScreen(const QString &role);

    void setUserId(int userId);
    int getUserId();

    void loadMarks(int studentId);
private slots:
    void on_pushButtonLogin_clicked();

private:
    Ui::MainWindow *ui;
    void mainWindowInit();
    int userId_;

};
#endif // MAINWINDOW_H
