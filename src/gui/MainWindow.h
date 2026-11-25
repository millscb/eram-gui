#pragma once
#include <QMainWindow>
#include "core/Simulation.h"

class QMdiArea;
class QLabel;
class QTimer;
class SectorView;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
private:
    void openSummary();
    void openProcessors();
    void openEvents();
    void openSector();
    void refreshDisplays();
    eram::Simulation simulation_;
    QMdiArea* desktop_ = nullptr;
    QLabel* banner_ = nullptr;
    QLabel* clock_ = nullptr;
    SectorView* sector_ = nullptr;
    QWidget* eventsPanel_ = nullptr;
    QTimer* timer_ = nullptr;
};
