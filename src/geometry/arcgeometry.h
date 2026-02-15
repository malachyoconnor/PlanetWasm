#ifndef ARCGEOMETRY_H
#define ARCGEOMETRY_H

#include <QQuick3DGeometry>
#include <QQmlEngine>

class ArcGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float startLatitude MEMBER m_startLat NOTIFY changed)
    Q_PROPERTY(float startLongitude MEMBER m_startLon NOTIFY changed)
    Q_PROPERTY(float endLatitude MEMBER m_endLat NOTIFY changed)
    Q_PROPERTY(float endLongitude MEMBER m_endLon NOTIFY changed)

public:
    explicit ArcGeometry(QQuick3DObject *parent = nullptr);

signals:
    void changed();

private:
    void rebuild();

    float m_startLat = 0, m_startLon = 0;
    float m_endLat = 0, m_endLon = 0;
};

#endif
