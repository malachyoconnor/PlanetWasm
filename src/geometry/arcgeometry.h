#ifndef ARCGEOMETRY_H
#define ARCGEOMETRY_H

#include <QQuick3DGeometry>
#include <QQmlEngine>
#include <QTimer>

class ArcGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double startLatitude MEMBER m_startLat NOTIFY changed)
    Q_PROPERTY(double startLongitude MEMBER m_startLon NOTIFY changed)
    Q_PROPERTY(double endLatitude MEMBER m_endLat NOTIFY changed)
    Q_PROPERTY(double endLongitude MEMBER m_endLon NOTIFY changed)
    Q_PROPERTY(float cameraZ MEMBER m_cameraZ NOTIFY changed)

public:
    explicit ArcGeometry(QQuick3DObject *parent = nullptr);

signals:
    void changed();

private:
    void rebuild();

    QTimer m_rebuildTimer;
    double m_startLat = 0, m_startLon = 0;
    double m_endLat = 0, m_endLon = 0;
    float m_cameraZ = 1.0f;
};

#endif
