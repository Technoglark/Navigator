#ifndef DRAWLAYER_H
#define DRAWLAYER_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <unordered_map>

enum Mode{
    ADDPOINT,
    SELECTNEIGHBOURS,
    SELECTPATH
};

class DrawLayer : public QWidget
{
    Q_OBJECT
public:
    explicit DrawLayer(QWidget* parent = nullptr);

    std::unordered_map<int, std::tuple<double, double, std::unordered_map<int, double>>> getPoints();

    void UpdateTable(QTableView* table);

    void SetTable(QTableView* table);

    void DrawPath(int start, int end);

    int FindPoint(int x, int y);

    void SetMode(int mode);

    int GetSelected();

    void AddNeighbours();

    void InternalUpdate();

    int getStart();

    int getEnd();




protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;



private:
    int pointNumber;
    std::unordered_map<int, std::tuple<double, double, std::unordered_map<int, double>>> points;
    QTableView* table;
    std::vector<int> min_path;
    Mode mode = Mode::ADDPOINT;
    std::unordered_map<int, double> neighbours;
    int selected = -1;
    int start = -1;
    int end = -1;
};

#endif // DRAWLAYER_H
