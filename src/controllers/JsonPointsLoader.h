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

static constexpr int DegreeDirectionChangeThreshold = 30;

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
        if (points.size() < 2) {
            qWarning() << "JsonPointsLoader: need at least 2 points";
            return;
        }

        // Group consecutive points by dateAsString
        struct DayGroup {
            QString date;
            QJsonArray points;
        };
        QVector<DayGroup> dayGroups;

        for (const auto& point : points) {
            auto obj = point.toObject();
            QString date = obj[QStringLiteral("dateAsString")].toString();

            if (dayGroups.isEmpty() || dayGroups.last().date != date) {
                dayGroups.append({date, {}});
            }
            dayGroups.last().points.append(obj);
        }

        static const QColor palette[] = {
            QColor(0xff6b6b), QColor(0x4ecdc4), QColor(0xffe66d), QColor(0xa8e6cf),
            QColor(0xff8b94), QColor(0x6c5ce7), QColor(0xfd79a8), QColor(0x00cec9),
            QColor(0xfab1a0), QColor(0x74b9ff), QColor(0x55efc4), QColor(0xffeaa7),
            QColor(0xdfe6e9), QColor(0xb2bec3), QColor(0xe17055), QColor(0x0984e3)
        };
        static constexpr int paletteSize = 16;
        static constexpr float DirectionChangeThreshold = qDegreesToRadians(DegreeDirectionChangeThreshold);

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
                 << points.size() << "points across" << dayGroups.size() << "day groups";
    }

private:
    QQmlEngine *m_engine;
    QObject *m_sceneRoot;
};

#endif // PLANETWASM_JSONPOINTSLOADER_H
