#pragma once
#include <QWidget>
#include "core/Simulation.h"

class SectorView : public QWidget {
public:
    explicit SectorView(eram::Simulation& simulation, QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    eram::Simulation& simulation_;
};
