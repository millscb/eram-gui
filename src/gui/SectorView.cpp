#include "gui/SectorView.h"
#include <QPainter>
#include <QPaintEvent>
#include <algorithm>
#include <cmath>

SectorView::SectorView(eram::Simulation& simulation, QWidget* parent)
    : QWidget(parent), simulation_(simulation) {
    setMinimumSize(540, 480);
    setAutoFillBackground(false);
}

void SectorView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#000511"));
    const int side = std::max(1, std::min(width(), height()) - 70);
    const QRectF area((width()-side)/2.0, (height()-side)/2.0, side, side);
    p.setPen(QPen(QColor("#254b45"), 1));
    p.drawRect(area);
    for (int i = 1; i < 4; ++i) {
        p.drawLine(QPointF(area.left()+side*i/4.0, area.top()),
                   QPointF(area.left()+side*i/4.0, area.bottom()));
        p.drawLine(QPointF(area.left(), area.top()+side*i/4.0),
                   QPointF(area.right(), area.top()+side*i/4.0));
    }
    p.setPen(QColor("#94a3b8"));
    p.drawText(QRectF(8, 8, width()-16, 28), Qt::AlignLeft,
               "SIMULATED R-POSITION  |  TRAINING / NOT FOR OPERATIONAL USE");
    p.setFont(QFont("Monospace", 10));
    p.setPen(QPen(QColor("#39ff14"), 1.5));
    for (const auto& t : simulation_.targets()) {
        const QPointF pt(area.left()+t.x*side, area.top()+t.y*side);
        p.drawEllipse(pt, 4, 4);
        const double rad = t.heading * 3.14159265358979323846 / 180.0;
        const QPointF endpoint(pt.x()+std::sin(rad)*35, pt.y()-std::cos(rad)*35);
        p.drawLine(pt, endpoint);
        const QPointF label(pt.x()+12, pt.y()-12);
        p.drawLine(pt, label);
        p.drawText(label+QPointF(3,-3), QString::fromStdString(t.callsign));
        p.drawText(label+QPointF(3,12),
                   QString("%1 %2").arg(t.assignedAltitude).arg(t.altitude));
        p.drawText(label+QPointF(3,27),
                   QString("%1 %2").arg(QString::fromStdString(t.cid)).arg(t.groundspeed));
    }
}
