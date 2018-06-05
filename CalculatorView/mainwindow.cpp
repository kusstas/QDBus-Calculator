#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDBus>
#include <cassert>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow),
    m_iface("org.kusstas.calculator", "/org/kusstas/calculator", "org.kusstas.ICalculator", QDBusConnection::sessionBus(), this)
{
    ui->setupUi(this);
    assert(QDBusConnection::sessionBus().isConnected());
    assert(m_iface.isValid());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnCalculate_clicked()
{
//    QDBusReply<QString> reply;
//    reply = m_iface.call("compute", ui->txtExpression->text());

//    if (reply.isValid()) {
//        ui->txtResult->setText(reply.value());
//    }
//    else {
//        qDebug() << "caclulate not call!";
//    }

    QDBusReply<double> reply;
    reply = m_iface.call("test", 5);

    if (reply.isValid()) {
        qDebug() << reply.value();
    }
    else {
        qDebug() << "caclulate not call!";
    }
}
