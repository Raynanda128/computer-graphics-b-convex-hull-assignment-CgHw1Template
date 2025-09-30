#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <cmath>

long long cross_product(QPoint a, QPoint b, QPoint c) {
    return (long long)(b.x() - a.x()) * (c.y() - a.y()) - (long long)(b.y() - a.y()) * (c.x() - a.x());
}

QVector<QPoint> findFastConvexHull(QVector<QPoint> points, long long& iterations) {
    iterations = 0;
    int n = points.size();
    if (n <= 2) return points;

    std::sort(points.begin(), points.end(), [](QPoint a, QPoint b) {
        return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y());
    });

    QVector<QPoint> lower_hull, upper_hull;

    for (const QPoint &p : points) {
        while (lower_hull.size() >= 2) {
            iterations++;
            if (cross_product(lower_hull[lower_hull.size()-2], lower_hull.back(), p) > 0) break;
            lower_hull.pop_back();
        }
        lower_hull.push_back(p);
    }

    for (int i = n - 1; i >= 0; --i) {
        const QPoint &p = points[i];
        while (upper_hull.size() >= 2) {
            iterations++;
            if (cross_product(upper_hull[upper_hull.size()-2], upper_hull.back(), p) > 0) break;
            upper_hull.pop_back();
        }
        upper_hull.push_back(p);
    }

    lower_hull.pop_back();
    upper_hull.pop_back();

    return lower_hull + upper_hull;
}

QVector<QPoint> findSlowConvexHull(const QVector<QPoint>& points, long long& iterations) {
    iterations = 0;
    if (points.size() < 3) return {};

    QVector<QPoint> hull_points;
    int n = points.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            bool all_on_one_side = true;
            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;

                iterations++;
                if (cross_product(points[i], points[j], points[k]) < 0) {
                    all_on_one_side = false;
                    break;
                }
            }

            if (all_on_one_side) {
                if (!hull_points.contains(points[i])) hull_points.push_back(points[i]);
                if (!hull_points.contains(points[j])) hull_points.push_back(points[j]);
            }
        }
    }

    if (!hull_points.isEmpty()) {
        QPoint center = hull_points[0];
        std::sort(hull_points.begin() + 1, hull_points.end(), [center](const QPoint& a, const QPoint& b) {
            return std::atan2(a.y() - center.y(), a.x() - center.x()) < std::atan2(b.y() - center.y(), b.x() - center.x());
        });
    }
    return hull_points;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Computer Graphics Simulation");

    QString outlineStyle = "QPushButton {"
                           "  background-color: transparent;"
                           "  color: #333333;"
                           "  border: 2px solid #555555;"
                           "  padding: 8px;"
                           "  border-radius: 8px;"
                           "  font-size: 14px;"
                           "}"
                           "QPushButton:hover {"
                           "  background-color: #555555;"
                           "  color: white;"
                           "}"
                           "QPushButton:pressed {"
                           "  background-color: #333333;"
                           "  border-color: #333333;"
                           "}";

    ui->runFastButton->setStyleSheet(outlineStyle);
    ui->runSlowButton->setStyleSheet(outlineStyle);
    ui->clearButton->setStyleSheet(outlineStyle.replace("#555555", "#A04040").replace("#333333", "#803030"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        clickedPoints.append(event->pos());
        update();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pointPen(Qt::red);
    pointPen.setWidth(8);
    painter.setPen(pointPen);
    for (const QPoint &point : clickedPoints) {
        painter.drawPoint(point);
    }

    if (!convexHullPoints.isEmpty() && convexHullPoints.size() > 1) {
        QPen linePen(Qt::blue);
        linePen.setWidth(2);
        painter.setPen(linePen);
        for (int i = 0; i < convexHullPoints.size(); ++i) {
            painter.drawLine(convexHullPoints[i], convexHullPoints[(i + 1) % convexHullPoints.size()]);
        }
    }

    if (!iterationCountText.isEmpty()) {
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(10, 20, iterationCountText);
    }
}

void MainWindow::on_runFastButton_clicked()
{
    if (clickedPoints.size() > 2) {
        long long iterations = 0;
        convexHullPoints = findFastConvexHull(clickedPoints, iterations);
        iterationCountText = QString("Fast Algorithm Iterations: %1").arg(iterations);
        update();
    }
}

void MainWindow::on_runSlowButton_clicked()
{
    if (clickedPoints.size() > 2) {
        long long iterations = 0;
        convexHullPoints = findSlowConvexHull(clickedPoints, iterations);
        iterationCountText = QString("Slow Algorithm Iterations: %1").arg(iterations);
        update();
    }
}

void MainWindow::on_clearButton_clicked()
{
    clickedPoints.clear();
    convexHullPoints.clear();
    iterationCountText.clear();
    update();
}
