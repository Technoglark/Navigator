#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include "drawlayer.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layer = new DrawLayer(this);
    layer->SetTable(ui->PointTable);
    layer->setAutoFillBackground(true);
    QPalette pal = layer->palette();
    pal.setColor(QPalette::Window, Qt::black);
    layer->setPalette(pal);
    ui->verticalLayout->addWidget(layer);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateTable(){
    auto points = layer->getPoints();
    QStandardItemModel *modelView = new QStandardItemModel(ui->PointTable);
    modelView->setHorizontalHeaderLabels({"Number", "Coordinates"});
    for(const auto& point : points){
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(point.first)
                 << new QStandardItem(std::get<0>(point.second))
                 << new QStandardItem(std::get<1>(point.second));
        modelView->appendRow(rowItems);
    }
    ui->PointTable->setModel(modelView);
    ui->PointTable->horizontalHeader()->setStretchLastSection(true);
    ui->PointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void MainWindow::on_pushButton_clicked()
{
    if(layer->getStart() != -1 && layer->getEnd() != -1){
        layer->DrawPath(layer->getStart(), layer->getEnd());
    }
    layer->SetMode(0);
}


void MainWindow::on_pushButton_2_clicked()
{
    layer->SetMode(1);
}


void MainWindow::on_pushButton_3_clicked()
{
    layer->AddNeighbours();
    layer->SetMode(0);
    layer->InternalUpdate();
}


void MainWindow::on_pushButton_4_clicked()
{
    layer->SetMode(2);
    layer->InternalUpdate();
}

