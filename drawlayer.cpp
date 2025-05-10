#include "drawlayer.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <queue>

int prev = -1;
std::vector<int> path;

DrawLayer::DrawLayer(QWidget* parent)
    : QWidget(parent)
{
    this->pointNumber = 0;
}

std::vector<int> Deikstra(std::unordered_map<int, std::tuple<double, double, std::unordered_map<int, double>>>& graph, int start, int end, int n) {
    std::unordered_map<int, int> path(n);
    std::unordered_map<int, double> ans(n);
    for (int i = 0; i != n; ++i) {
        path[i] = -1;
        ans[i] = INT_MAX;
    }

    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, std::greater<>> q;

    ans[start] = 0;
    q.push({start, 0});
    while (!q.empty()) {
        auto current = q.top();
        q.pop();
        double dist = current.second, v = current.first;

        if (ans[v] < dist) {
            continue;
        }

        for (auto x : std::get<2>(graph[current.first])) {
            double len_vu = x.second;
            int u = x.first;
            double n_dst = dist + len_vu;
            if (n_dst < ans[u]) {
                ans[u] = n_dst;
                path[u] = v;
                q.push({u, n_dst});
            }
        }
    }

    std::vector<int> result_path;
    int cur = end;
    while (path[cur] != -1) {
        result_path.push_back(cur);
        cur = path[cur];
    }
    result_path.push_back(cur);
    reverse(result_path.begin(), result_path.end());
    return result_path;
}



void DrawLayer::paintEvent(QPaintEvent* event){
    auto painter = new QPainter(this);
    painter->setPen(QPen(Qt::red, 2));
    painter->setBrush(Qt::red);


    for(auto& point : points){
        for(auto current : std::get<2>(point.second)){
            QPoint start = QPoint(std::get<0>(point.second), std::get<1>(point.second));
            QPoint end = QPoint(std::get<0>(points[current.first]), std::get<1>(points[current.first]));
            painter->drawLine(start, end);
        }
    }

    for(const auto& point : points){
        painter->drawPoint(QPoint(std::get<0>(point.second), std::get<1>(point.second)));
        painter->drawEllipse(QRect(QPoint(std::get<0>(point.second) - 10, std::get<1>(point.second) - 10), QPoint(std::get<0>(point.second) + 10, std::get<1>(point.second) + 10)));
    }

    if(start != -1){
        painter->setPen(QPen(Qt::cyan, 2));
        painter->setBrush(Qt::cyan);
        QPoint start_point = QPoint(std::get<0>(points[start]), std::get<1>(points[start]));
        painter->drawEllipse(QRect(QPoint(start_point.x() - 10, start_point.y() - 10), QPoint(start_point.x() + 10, start_point.y() + 10)));
    }
    if(end != -1){
        painter->setPen(QPen(Qt::white, 2));
        painter->setBrush(Qt::white);
        QPoint start_point = QPoint(std::get<0>(points[end]), std::get<1>(points[end]));
        painter->drawEllipse(QRect(QPoint(start_point.x() - 10, start_point.y() - 10), QPoint(start_point.x() + 10, start_point.y() + 10)));
    }

    if(!path.empty()){
        painter->setPen(QPen(Qt::green, 2));
        for(int i = 1; i != path.size(); ++i){
            QPoint start = QPoint(std::get<0>(points[path[i - 1]]), std::get<1>(points[path[i - 1]]));
            QPoint end = QPoint(std::get<0>(points[path[i]]), std::get<1>(points[path[i]]));
            painter->drawLine(QLine(start, end));
        }
        start = -1;
        end = -1;
        SetMode(0);
    }
    painter->setBrush(Qt::green);
    painter->setPen(QPen(Qt::green, 2));
    for(auto point : neighbours){
        painter->drawEllipse(QRect(QPoint(std::get<0>(points[point.first]) - 10, std::get<1>(points[point.first]) - 10), QPoint(std::get<0>(points[point.first]) + 10, std::get<1>(points[point.first]) + 10)));
    }
    if(selected != -1 && this->mode != Mode::SELECTPATH){
        painter->setPen(QPen(Qt::blue, 2));
        painter->setBrush(Qt::blue);
        painter->drawEllipse(QRect(QPoint(std::get<0>(points[selected]) - 10, std::get<1>(points[selected]) - 10), QPoint(std::get<0>(points[selected]) + 10, std::get<1>(points[selected]) + 10)));
    }
    path.clear();

}

void DrawLayer::mousePressEvent(QMouseEvent* event){
    auto pos = event->pos();
    int finded = FindPoint(pos.x(), pos.y());
    if(finded != -1 && selected == -1){
        selected = finded;
    }
    if(this->mode == Mode::SELECTNEIGHBOURS && finded != -1){
        if(finded != -1){
            QPoint start = QPoint(std::get<0>(points[selected]), std::get<1>(points[selected]));
            QPoint end = QPoint(std::get<0>(points[finded]), std::get<1>(points[finded]));
            double distance = std::sqrt((start.x() - end.x()) * (start.x() - end.x()) + (start.y() - end.y()) * (start.y() - end.y()));
            neighbours[finded] = distance;
        }
    } else if(this->mode == Mode::ADDPOINT && finded == -1){
        points[pointNumber++] = {static_cast<double>(pos.x()), static_cast<double>(pos.y()), {}};
    } else if(this->mode == Mode::SELECTPATH && start == -1){
        start = finded;
    } else if(this->mode == Mode::SELECTPATH && end == -1){
        end = finded;
    }
    qDebug() << "selected: "<< selected;
    for(auto x : neighbours){
        qDebug() << x.first;
    }
    UpdateTable(table);
    update();
}

void DrawLayer::mouseMoveEvent(QMouseEvent* event){
    return;
}

std::unordered_map<int, std::tuple<double, double, std::unordered_map<int, double>>> DrawLayer::getPoints(){
    return points;
}

void DrawLayer::SetTable(QTableView* table){
    this->table = table;
}

void DrawLayer::UpdateTable(QTableView* table){
    QStandardItemModel *modelView = new QStandardItemModel(table);
    modelView->setHorizontalHeaderLabels({"Number", "x", "y"});
    for(const auto& point : points){
        QList<QStandardItem*> rowItems;
        rowItems << new QStandardItem(QString::number(point.first))
                 << new QStandardItem(QString::number(std::get<0>(point.second)))
                 << new QStandardItem(QString::number(std::get<1>(point.second)));
        modelView->appendRow(rowItems);
    }
    table->setModel(modelView);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}



void DrawLayer::DrawPath(int start, int end){
    selected = -1;
    auto vertices = Deikstra(points, start, end, points.size());
    path = vertices;
    update();
}

int DrawLayer::FindPoint(int x, int y){
    double x1 = static_cast<double>(x);
    double y1 = static_cast<double>(y);
    for(auto point : points){
        double current_x = std::get<0>(point.second);
        double current_y = std::get<1>(point.second);
        if(abs(current_x - x) <= 10 && abs(current_y - y) < 10){
            return point.first;
        }
    }
    return -1;
}

void DrawLayer::SetMode(int mode){
    if(mode == 0){
        this->mode = Mode::ADDPOINT;
    } else if(mode == 1){
        this->mode = Mode::SELECTNEIGHBOURS;
    } else if(mode == 2){
        this->mode = Mode::SELECTPATH;
    }
}

int DrawLayer::GetSelected(){
    return selected;
}

void DrawLayer::AddNeighbours(){
    if(selected == -1){
        return;
    }
    for(auto x : neighbours){
        std::get<2>(points[selected])[x.first] = x.second;
        std::get<2>(points[x.first])[selected] = x.second;
    }
    neighbours.clear();
    selected = -1;
}

void DrawLayer::InternalUpdate(){
    update();
}

int DrawLayer::getStart(){
    return start;
}

int DrawLayer::getEnd(){
    return end;
}


