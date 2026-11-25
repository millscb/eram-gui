#include "gui/MainWindow.h"
#include "gui/SectorView.h"
#include <QAction>
#include <QDateTime>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

namespace {
QString healthLabel(eram::Health health) {
    switch (health) {
        case eram::Health::Up: return "UP";
        case eram::Health::Degraded: return "DEGRADED";
        case eram::Health::Down: return "DOWN";
    }
    return "UNKNOWN";
}
QWidget* wrapInScroll(QWidget* widget) {
    auto* scroll = new QScrollArea;
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);
    return scroll;
}
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("ERAM GUI Simulator — TRAINING ONLY");
    resize(1250, 850);
    setStyleSheet(R"(
        QMainWindow, QMdiArea, QScrollArea, QWidget { background: #0b132b; color: #f0f4f8; }
        QToolBar, QMenuBar, QMenu { background: #1c2541; color: #f0f4f8; }
        QPushButton { background: #2a4d69; color: white; padding: 6px; border: 1px solid #3a506b; }
        QPushButton:hover { background: #3a506b; }
        QTableWidget { background: #101c32; gridline-color: #3a506b; }
        QHeaderView::section { background: #3a506b; color: white; }
        QMdiSubWindow { background: #1c2541; }
    )");

    desktop_ = new QMdiArea(this);
    setCentralWidget(desktop_);
    auto* top = new QToolBar("System", this);
    top->setMovable(false);
    addToolBar(top);
    banner_ = new QLabel;
    top->addWidget(banner_);
    top->addSeparator();
    clock_ = new QLabel;
    top->addWidget(clock_);
    top->addSeparator();
    top->addWidget(new QLabel("Site: ZDC (SIMULATED)"));
    auto* actions = addToolBar("Workstations");
    actions->setMovable(false);
    auto add = [&](const char* label, void (MainWindow::*handler)()) {
        QAction* action = actions->addAction(label);
        connect(action, &QAction::triggered, this, handler);
    };
    add("System Summary", &MainWindow::openSummary);
    add("Processors", &MainWindow::openProcessors);
    add("Events", &MainWindow::openEvents);
    add("Sector Display", &MainWindow::openSector);
    actions->addSeparator();
    auto* reset = actions->addAction("Reset Simulation");
    connect(reset, &QAction::triggered, this, [this] {
        simulation_.reset();
        refreshDisplays();
    });

    auto* simulationMenu = menuBar()->addMenu("Simulation");
    for (auto pair : {std::pair{"Channel A", eram::Channel::A}, {"Channel B", eram::Channel::B}}) {
        auto* action = simulationMenu->addAction(pair.first);
        connect(action, &QAction::triggered, this, [this, channel = pair.second] {
            simulation_.setActiveChannel(channel);
            refreshDisplays();
        });
    }
    auto* about = menuBar()->addAction("About");
    connect(about, &QAction::triggered, this, [this] {
        QMessageBox::information(this, "About", "ERAM GUI Simulator 0.1.0\nOffline synthetic training data only.\nNot affiliated with FAA.");
    });

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this] {
        simulation_.step(1);
        refreshDisplays();
    });
    timer_->start(1000);
    openSummary();
    openSector();
    refreshDisplays();
}

void MainWindow::openSummary() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel("SIMULATED SYSTEM STATUS"));
    for (const char* name : {"Surveillance", "Flight Data", "Weather", "Strategic Alert",
                             "Channel A", "Channel B", "National Applications"}) {
        auto* row = new QLabel(QString("%1:  UP").arg(name));
        row->setStyleSheet("color: #00ff66; font-weight: bold;");
        layout->addWidget(row);
    }
    layout->addStretch();
    auto* window = desktop_->addSubWindow(widget);
    window->setWindowTitle("System Summary");
    window->resize(360, 320);
    widget->show();
}

void MainWindow::openProcessors() {
    auto* table = new QTableWidget(static_cast<int>(simulation_.processors().size()), 4);
    table->setHorizontalHeaderLabels({"ID", "Name", "Group", "Health"});
    for (int row = 0; row < static_cast<int>(simulation_.processors().size()); ++row) {
        const auto& processor = simulation_.processors()[row];
        const QString cells[] = {QString::fromStdString(processor.id),
                                 QString::fromStdString(processor.name),
                                 QString::fromStdString(processor.category),
                                 healthLabel(processor.health)};
        for (int col = 0; col < 4; ++col) {
            auto* item = new QTableWidgetItem(cells[col]);
            if (col == 3) item->setForeground(QColor("#00ff66"));
            table->setItem(row, col, item);
        }
    }
    auto* window = desktop_->addSubWindow(table);
    window->setWindowTitle("Processors - Simulated");
    window->resize(630, 410);
    table->show();
}

void MainWindow::openEvents() {
    auto* panel = new QWidget;
    auto* layout = new QVBoxLayout(panel);
    auto* table = new QTableWidget(static_cast<int>(simulation_.events().size()), 3, panel);
    table->setHorizontalHeaderLabels({"Event", "Message", "ACK"});
    for (int row=0; row<static_cast<int>(simulation_.events().size()); ++row) {
        const auto& event = simulation_.events()[row];
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(event.id)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(event.message)));
        table->setItem(row, 2, new QTableWidgetItem(event.acknowledged ? "YES" : "NO"));
    }
    layout->addWidget(table);
    auto* button = new QPushButton("Acknowledge All", panel);
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, panel, [this, table] {
        for (const auto& event : simulation_.events()) simulation_.acknowledgeEvent(event.id);
        for (int i=0; i<table->rowCount(); ++i) table->item(i, 2)->setText("YES");
    });
    auto* window = desktop_->addSubWindow(panel);
    window->setWindowTitle("Event Log - Synthetic");
    window->resize(700, 350);
    panel->show();
}

void MainWindow::openSector() {
    auto* panel = new QWidget;
    auto* layout = new QVBoxLayout(panel);
    auto* label = new QLabel("R-POSITION  |  Synthetic traffic  |  600 NM normalized viewport");
    layout->addWidget(label);
    auto* view = new SectorView(simulation_, panel);
    layout->addWidget(view, 1);
    auto* toolbar = new QHBoxLayout;
    auto* step = new QPushButton("Advance 60 s");
    toolbar->addWidget(step);
    auto* reset = new QPushButton("Reset Traffic");
    toolbar->addWidget(reset);
    layout->addLayout(toolbar);
    connect(step, &QPushButton::clicked, panel, [this, view] {
        simulation_.step(60);
        view->update();
    });
    connect(reset, &QPushButton::clicked, panel, [this, view] {
        simulation_.reset();
        refreshDisplays();
        view->update();
    });
    connect(panel, &QObject::destroyed, this, [this, panel] {
        if (sector_ && sector_->parentWidget() == panel) sector_ = nullptr;
    });
    sector_ = view;
    auto* window = desktop_->addSubWindow(panel);
    window->setWindowTitle("Sector Situation Display — SIMULATION");
    window->resize(740, 660);
    panel->show();
    window->move(380, 35);
}

void MainWindow::refreshDisplays() {
    banner_->setText(simulation_.activeChannel() == eram::Channel::A
        ? "Channel A: ACTIVE   |   B: BACKUP IN SYNC"
        : "Channel B: ACTIVE   |   A: BACKUP IN SYNC");
    clock_->setText(QDateTime::currentDateTimeUtc().toString("HH:mm:ss 'UTC'"));
    if (sector_) sector_->update();
}
