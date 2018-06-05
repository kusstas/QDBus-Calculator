#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDBus>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCalculate_clicked();

private:
    Ui::MainWindow* ui;
    QDBusInterface m_iface;
};

#endif // MAINWINDOW_H
