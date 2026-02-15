#ifndef PLANETWASM_ARCMANAGER_H
#define PLANETWASM_ARCMANAGER_H
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
   auto materials = model->property("materials");
   if (materials.isValid()) {
      auto matList = materials.value<QList<QObject *> >();
      if (!matList.isEmpty())
         matList.first()->setProperty("baseColor", color);
   }
}


#endif //PLANETWASM_ARCMANAGER_H