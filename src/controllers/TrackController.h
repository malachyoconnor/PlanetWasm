#ifndef PLANETWASM_TRACKCONTROLLER_H
#define PLANETWASM_TRACKCONTROLLER_H

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlListReference>
#include <QQuick3DObject>
#include <QVariantList>
#include <QColor>

static void addTrack(QQmlEngine *engine, QObject *sceneRoot,
                     const QVariantList &points, const QColor &color) {
    QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/qml/components/track.qml")));

    if (component.isError()) {
        for (const auto &e : component.errors())
            qWarning() << "QML track error:" << e.toString();
        return;
    }

    auto *model = qobject_cast<QQuick3DObject *>(component.create());
    if (!model) {
        qWarning() << "Failed to create track Model";
        return;
    }

    model->setParentItem(qobject_cast<QQuick3DObject *>(sceneRoot));

    // Set points on the TrackGeometry child
    auto *geom = model->findChild<QObject *>();
    if (geom && geom->metaObject()->className() == QStringLiteral("TrackGeometry")) {
        geom->setProperty("points", points);
    } else {
        // Fallback: search by property name
        for (auto *child : model->children()) {
            if (child->property("points").isValid()) {
                child->setProperty("points", points);
                break;
            }
        }
    }

    // Set the material color
    QQmlListReference materials(model, "materials");
    if (materials.count() > 0) {
        materials.at(0)->setProperty("baseColor", color);
    }
}

#endif // PLANETWASM_TRACKCONTROLLER_H
