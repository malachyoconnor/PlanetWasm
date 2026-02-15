#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "controllers/ArcController.h"
#include "controllers/ArcBridge.h"
#include "controllers/JsonPointsLoader.h"
#include "controllers/LineBridge.h"

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

   // Useful debug arc from London to New York
   // addArc(&engine, sceneRoot, 51.5, -0.1, 40.7, -74.0, QColor(0xff6b6b));

   // Expose bridge to QML for interactive arc creation
   auto arcBridge = std::make_unique<ArcBridge>(&engine, sceneRoot, &app);
   engine.rootContext()->setContextProperty("arcBridge", arcBridge.get());

   // Expose JSON points loader for drag-and-drop import
   auto pointsLoader = std::make_unique<JsonPointsLoader>(&engine, sceneRoot, &app);
   engine.rootContext()->setContextProperty("pointsLoader", pointsLoader.get());

   // Bridge for drawing straight lines, useful for debugging
   auto lineBridge = std::make_unique<LineBridge>(&engine, sceneRoot, &app);
   engine.rootContext()->setContextProperty("lineBridge", lineBridge.get());


   return app.exec();
}
