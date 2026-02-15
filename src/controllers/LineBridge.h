#ifndef PLANETWASM_LINEBRIDGE_H
#define PLANETWASM_LINEBRIDGE_H

#include <QObject>
#include <QColor>
#include "LineController.h"

class LineBridge : public QObject {
    Q_OBJECT
public:
    explicit LineBridge(QQmlEngine *engine, QObject *sceneRoot, QObject *parent = nullptr)
        : QObject(parent), m_engine(engine), m_sceneRoot(sceneRoot) {}

    Q_INVOKABLE void addLineFromUI(float sx, float sy, float sz, float ex, float ey, float ez, const QString &color) {
        addLine(m_engine, m_sceneRoot, {sx, sy, sz}, {ex, ey, ez}, QColor(color));
    }

private:
    QQmlEngine *m_engine;
    QObject *m_sceneRoot;
};

#endif // PLANETWASM_LINEBRIDGE_HH
