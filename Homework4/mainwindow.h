#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_runFastButton_clicked();
    void on_runSlowButton_clicked();
    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QPoint> clickedPoints;
    QVector<QPoint> convexHullPoints;
    QString iterationCountText;
};

#endif
