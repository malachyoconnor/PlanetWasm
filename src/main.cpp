#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuick3DObject>

#include "controllers/ArcController.h"
#include "controllers/ArcBridge.h"
#include "controllers/JsonPointsLoader.h"

int main(int argc, char *argv[]) {
   QGuiApplication app(argc, argv);

   QQmlApplicationEngine engine;
   engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

   if (engine.rootObjects().isEmpty())
      return -1;

   // Find the scene root node we exposed in QML
   QObject *root = engine.rootObjects().first();
   QObject *sceneRoot = root->findChild<QObject *>(QStringLiteral("sceneRoot"));

   if (!sceneRoot) {
      qFatal("ERROR: sceneRoot not found");
   }

   // // London to New York
   addArc(&engine, sceneRoot, 51.5, -0.1, 40.7, -74.0, QColor(0xff6b6b));
   // // Tokyo to Sydney
   // addArc(&engine, sceneRoot, 35.7, 139.7, -33.9, 151.2, QColor(0x4ecdc4));

   // Expose bridge to QML for interactive arc creation
   auto arcBridge = std::make_unique<ArcBridge>(&engine, sceneRoot, &app);
   engine.rootContext()->setContextProperty("arcBridge", arcBridge.get());

   // Expose JSON points loader for drag-and-drop import
   auto pointsLoader = std::make_unique<JsonPointsLoader>(&engine, sceneRoot, &app);
   engine.rootContext()->setContextProperty("pointsLoader", pointsLoader.get());

   return app.exec();
}