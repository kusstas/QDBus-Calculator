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

void MainWindow::on_txtExpression_textChanged(QString const& txt)
{
    QDBusReply<QString> reply;
    reply = m_iface.call("compute", txt);

    if (reply.isValid()) {
        ui->txtResult->setText(reply.value());
    }
    else {
        qDebug() << "caclulate not call!";
    }
}
