#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "thread.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include "gridcoords.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // cooridnates
    int gridwidth, gridheight;
    void setColWidth(int col);
    int getColWidth();
    void setRowWidth(int row);
    int getRowWidth();
    bool leftpressed, rightpressed;
    QPoint MainPoints, startPoint, endPoint;
    QPoint grid_startPoint, grid_endPoint;
    bool startlocation, endlocation;
    bool startlocationSet, endlocationSet;
    int obstaclecount;
    bool selectobstacle, obstaclesSet;
    QPoint obstaclePoint, obstacles[100], grid_obstacles[100];


public slots:
    void thread_byte(unsigned char);
    void received_frame(unsigned char, int, unsigned char*);

private:
    Ui::MainWindow *ui;
    int colwidth;
    int rowwidth;
    Thread *thread;

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *mev);
private slots:
    void on_start_loc_Button_clicked();
    void on_end_loc_Button_clicked();
    void on_start_obstacle_Button_clicked();
    void on_set_obs_Button_clicked();
    void on_debug_Button_clicked();
    void on_path_plan_Button_clicked();

};

#endif // MAINWINDOW_H
