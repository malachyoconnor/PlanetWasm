#ifndef PLANETWASM_ARCMANAGER_H
#define PLANETWASM_ARCMANAGER_H
#include <QQmlListReference>
#include "geometry/arcgeometry.h"

static void addArc(QQmlEngine *engine, QObject *sceneRoot,
                   float lat1, float lon1, float lat2, float lon2,
                   const QColor &color) {
   QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/qml/components/arc.qml")));

   if (component.isError()) {
      for (const auto &e: component.errors())
         qWarning() << "QML arc error:" << e.toString();
      return;
   }

   auto *model = qobject_cast<QQuick3DObject *>(component.create());
   if (!model) {
      qWarning() << "Failed to create arc Model";
      return;
   }

   model->setParentItem(qobject_cast<QQuick3DObject *>(sceneRoot));

   auto *arc = model->findChild<ArcGeometry *>();
   if (arc) {
      arc->setProperty("startLatitude", lat1);
      arc->setProperty("startLongitude", lon1);
      arc->setProperty("endLatitude", lat2);
      arc->setProperty("endLongitude", lon2);
   }

   // Set the material color
   QQmlListReference materials(model, "materials");
   if (materials.count() > 0) {
      materials.at(0)->setProperty("baseColor", color);
   } else {
      qDebug() << "No materials found for arc. Couldn't set colour";
   }
}


#endif //PLANETWASM_ARCMANAGER_H