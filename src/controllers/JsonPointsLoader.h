#ifndef PLANETWASM_JSONPOINTSLOADER_H
#define PLANETWASM_JSONPOINTSLOADER_H

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>
#include <QColor>
#include <QtMath>
#include "TrackController.h"
#include "geometry/utils.h"

class JsonPointsLoader : public QObject {
    Q_OBJECT
public:
    explicit JsonPointsLoader(QQmlEngine *engine, QObject *sceneRoot, QObject *parent = nullptr)
        : QObject(parent), m_engine(engine), m_sceneRoot(sceneRoot) {}

    Q_INVOKABLE void loadFile(const QUrl &fileUrl) {
        QFile file(fileUrl.toLocalFile());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "JsonPointsLoader: cannot open" << fileUrl;
            return;
        }

        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(file.readAll(), &err);
        if (err.error != QJsonParseError::NoError) {
            qWarning() << "JsonPointsLoader: parse error:" << err.errorString();
            return;
        }

        if (!doc.isArray()) {
            qWarning() << "JsonPointsLoader: expected a JSON array";
            return;
        }

        QJsonArray points = doc.array();
        int n = points.size();
        if (n < 2) {
            qWarning() << "JsonPointsLoader: need at least 2 points";
            return;
        }

        // Group consecutive points by dateAsString
        struct DayGroup {
            QString date;
            QJsonArray points;
        };
        QVector<DayGroup> dayGroups;

        for (int i = 0; i < n; ++i) {
            auto obj = points[i].toObject();
            QString date = obj[QStringLiteral("dateAsString")].toString();

            if (dayGroups.isEmpty() || dayGroups.last().date != date) {
                dayGroups.append({date, {}});
            }
            dayGroups.last().points.append(obj);
        }

        static const QColor palette[] = {
            QColor("#ff6b6b"), QColor("#4ecdc4"), QColor("#ffe66d"), QColor("#a8e6cf"),
            QColor("#ff8b94"), QColor("#6c5ce7"), QColor("#fd79a8"), QColor("#00cec9"),
            QColor("#fab1a0"), QColor("#74b9ff"), QColor("#55efc4"), QColor("#ffeaa7"),
            QColor("#dfe6e9"), QColor("#b2bec3"), QColor("#e17055"), QColor("#0984e3")
        };
        static constexpr int paletteSize = 16;
        static constexpr float DirectionChangeThreshold = 0.52f; // ~30 degrees in radians

        int trackCount = 0;

        for (const auto &group : dayGroups) {
            QColor color = palette[qHash(group.date) % paletteSize];

            // Convert group points to cartesian for direction analysis
            struct GeoPoint {
                double lat, lon;
                QVector3D cart;
            };
            QVector<GeoPoint> geoPoints;
            geoPoints.reserve(group.points.size());

            for (int i = 0; i < group.points.size(); ++i) {
                auto obj = group.points[i].toObject();
                double lat = obj[QStringLiteral("latitude")].toDouble();
                double lon = obj[QStringLiteral("longitude")].toDouble();
                QVector3D cart = geometry::toCartesian(lat, lon).normalized();

                // Skip near-coincident points
                if (!geoPoints.isEmpty() &&
                    (cart - geoPoints.last().cart).lengthSquared() < 1e-12f)
                    continue;

                geoPoints.append({lat, lon, cart});
            }

            if (geoPoints.size() < 2)
                continue;

            // Split into strips by direction change
            QVector<int> stripStarts;
            stripStarts.append(0);

            for (int i = 1; i < geoPoints.size() - 1; ++i) {
                QVector3D dirIn = (geoPoints[i].cart - geoPoints[i - 1].cart).normalized();
                QVector3D dirOut = (geoPoints[i + 1].cart - geoPoints[i].cart).normalized();

                float dot = std::clamp(QVector3D::dotProduct(dirIn, dirOut), -1.0f, 1.0f);
                float angle = std::acos(dot);

                if (angle > DirectionChangeThreshold) {
                    stripStarts.append(i);
                }
            }

            // Create a track for each strip
            for (int s = 0; s < stripStarts.size(); ++s) {
                int start = stripStarts[s];
                int end = (s + 1 < stripStarts.size()) ? stripStarts[s + 1] : geoPoints.size() - 1;

                if (end - start < 1)
                    continue;

                QVariantList trackPoints;
                trackPoints.reserve(end - start + 1);
                for (int i = start; i <= end; ++i) {
                    QVariantMap pt;
                    pt[QStringLiteral("latitude")] = geoPoints[i].lat;
                    pt[QStringLiteral("longitude")] = geoPoints[i].lon;
                    trackPoints.append(pt);
                }

                addTrack(m_engine, m_sceneRoot, trackPoints, color);
                ++trackCount;
            }
        }

        qDebug() << "JsonPointsLoader: created" << trackCount << "tracks from"
                 << n << "points across" << dayGroups.size() << "day groups";
    }

private:
    QQmlEngine *m_engine;
    QObject *m_sceneRoot;
};

#endif // PLANETWASM_JSONPOINTSLOADER_H
