#ifndef STRAIGHTLINE_H
#define STRAIGHTLINE_H

#include <QQuick3DGeometry>
#include <QQmlEngine>
#include <QVector3D>

class StraightLine : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVector3D start MEMBER m_start NOTIFY changed)
    Q_PROPERTY(QVector3D end MEMBER m_end NOTIFY changed)

public:
    explicit StraightLine(QQuick3DObject *parent = nullptr);

signals:
    void changed();

private:
    void rebuild();

   QVector3D m_start {};
   QVector3D m_end {};
};

#endif
