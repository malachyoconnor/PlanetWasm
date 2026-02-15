#ifndef PLANETWASM_ARCBRIDGE_H
#define PLANETWASM_ARCBRIDGE_H

#include <QObject>
#include <QQmlEngine>
#include <QColor>
#include "ArcController.h"

class ArcBridge : public QObject {
    Q_OBJECT
public:
    explicit ArcBridge(QQmlEngine *engine, QObject *sceneRoot, QObject *parent = nullptr)
        : QObject(parent), m_engine(engine), m_sceneRoot(sceneRoot) {}

    Q_INVOKABLE void addArcFromUI(float lat1, float lon1, float lat2, float lon2, const QString &color) {
        addArc(m_engine, m_sceneRoot, lat1, lon1, lat2, lon2, QColor(color));
    }

private:
    QQmlEngine *m_engine;
    QObject *m_sceneRoot;
};

#endif // PLANETWASM_ARCBRIDGE_H
