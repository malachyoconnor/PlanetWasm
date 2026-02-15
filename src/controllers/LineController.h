#ifndef PLANETWASM_LINECONTROLLER_H
#define PLANETWASM_LINECONTROLLER_H

#include <QQmlListReference>
#include "geometry/straightline.h"


static void addLine(QQmlEngine *engine, QObject *sceneRoot, QVector3D start, QVector3D end, const QColor &color) {
   QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/qml/components/line.qml")));

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

   auto *line = model->findChild<StraightLine *>();
   if (line) {
      line->setProperty("start", start);
      line->setProperty("end", end);
   }

   // Set the material color
   QQmlListReference materials(model, "materials");
   if (materials.count() > 0) {
      materials.at(0)->setProperty("baseColor", color);
   } else {
      qDebug() << "No materials found for arc. Couldn't set colour";
   }
}

#endif //PLANETWASM_LINECONTROLLER_H