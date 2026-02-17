#ifndef TRACKGEOMETRY_H
#define TRACKGEOMETRY_H

#include <QQuick3DGeometry>
#include <QQmlEngine>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>
#include <limits>
#include "utils.h"
#include "constants.h"

class TrackGeometry : public QQuick3DGeometry {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList points READ points WRITE setPoints NOTIFY pointsChanged)

public:
    explicit TrackGeometry(QQuick3DObject *parent = nullptr)
        : QQuick3DGeometry(parent) {
        m_rebuildTimer.setSingleShot(true);
        m_rebuildTimer.setInterval(50);
        connect(&m_rebuildTimer, &QTimer::timeout, this, &TrackGeometry::rebuild);
        connect(this, &TrackGeometry::pointsChanged, &m_rebuildTimer, qOverload<>(&QTimer::start));
    }

    QVariantList points() const { return m_points; }
    void setPoints(const QVariantList &pts) {
        m_points = pts;
        emit pointsChanged();
    }

signals:
    void pointsChanged();

private:
    static constexpr float BaseOffset = 0.001f;
    static constexpr float SubdivisionThreshold = 0.02f;

    void rebuild() {
        // Parse points to cartesian, deduplicating near-coincident ones
        struct CartPoint { QVector3D pos; };
        QVector<CartPoint> cartPoints;
        cartPoints.reserve(m_points.size());

        for (const auto &v : m_points) {
            auto map = v.toMap();
            double lat = map[QStringLiteral("latitude")].toDouble();
            double lon = map[QStringLiteral("longitude")].toDouble();
            QVector3D p = geometry::toCartesian(lat, lon).normalized();

            if (!cartPoints.isEmpty()) {
                float dist = (p - cartPoints.last().pos).lengthSquared();
                if (dist < 1e-12f)
                    continue;
            }
            cartPoints.append({p});
        }

        if (cartPoints.size() < 2) {
            clear();
            update();
            return;
        }

        // Build subdivided path points
        QVector<QVector3D> path;
        path.reserve(cartPoints.size() * 4);
        path.append(cartPoints[0].pos);

        for (int i = 0; i < cartPoints.size() - 1; ++i) {
            const QVector3D &a = cartPoints[i].pos;
            const QVector3D &b = cartPoints[i + 1].pos;

            float cosTheta = std::clamp(QVector3D::dotProduct(a, b), -1.0f, 1.0f);
            float angle = std::acos(cosTheta);
            int subdivisions = std::max(1, static_cast<int>(std::ceil(angle / SubdivisionThreshold)));

            for (int s = 1; s <= subdivisions; ++s) {
                float t = static_cast<float>(s) / subdivisions;
                path.append(geometry::interpolateSpherical(a, b, t).normalized());
            }
        }

        // Compute forward directions with smooth transitions at joints
        QVector<QVector3D> forwards(path.size());
        for (int i = 0; i < path.size(); ++i) {
            QVector3D fwd;
            if (i == 0) {
                fwd = (path[1] - path[0]);
            } else if (i == path.size() - 1) {
                fwd = (path[i] - path[i - 1]);
            } else {
                QVector3D incoming = (path[i] - path[i - 1]).normalized();
                QVector3D outgoing = (path[i + 1] - path[i]).normalized();
                fwd = (incoming + outgoing);
            }
            if (fwd.lengthSquared() < 1e-12f) {
                // Degenerate — use perpendicular to radial
                fwd = QVector3D::crossProduct(path[i], QVector3D(0, 1, 0));
                if (fwd.lengthSquared() < 1e-6f)
                    fwd = QVector3D::crossProduct(path[i], QVector3D(1, 0, 0));
            }
            forwards[i] = fwd.normalized();
        }

        // Build triangle strip
        clear();

        const int totalVertices = path.size() * 2;
        const int stride = 3 * sizeof(float);
        QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
        auto *writer = reinterpret_cast<float *>(vertexBuffer.data());

        // Compute total arc length on sphere for length-dependent width
        float totalLength = 0.0f;
        for (int i = 1; i < path.size(); ++i)
            totalLength += (path[i] - path[i - 1]).length();

        // Width as a fraction of track length, clamped to reasonable bounds
        static constexpr float WidthFraction = 0.03f;    // 3% of track length
        static constexpr float MinLineWidth  = 0.000001f;
        static constexpr float MaxLineWidth  = 0.0006f;
        float halfWidth = std::clamp(totalLength * WidthFraction, MinLineWidth, MaxLineWidth) * 0.5f;
        float radius = constants::SphereRadius + BaseOffset;

       // If we're on the order of a city
       if (geometry::arcLengthToMiles(totalLength) <= 15.0f) {
         halfWidth = MinLineWidth;
       }

        QVector3D minBound(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max());
        QVector3D maxBound(std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest());

        QVector3D prevSide;
        for (int i = 0; i < path.size(); ++i) {
            QVector3D side = QVector3D::crossProduct(path[i], forwards[i]).normalized();

            // Ensure consistent side direction — flip if it reversed from previous
            if (i > 0 && QVector3D::dotProduct(side, prevSide) < 0.0f)
                side = -side;
            prevSide = side;

            side *= halfWidth;

            QVector3D left = path[i] * radius + side;
            QVector3D right = path[i] * radius - side;

            *writer++ = left.x();  *writer++ = left.y();  *writer++ = left.z();
            *writer++ = right.x(); *writer++ = right.y(); *writer++ = right.z();

            for (const auto &v : {left, right}) {
                minBound = QVector3D(std::min(minBound.x(), v.x()),
                                     std::min(minBound.y(), v.y()),
                                     std::min(minBound.z(), v.z()));
                maxBound = QVector3D(std::max(maxBound.x(), v.x()),
                                     std::max(maxBound.y(), v.y()),
                                     std::max(maxBound.z(), v.z()));
            }
        }

        setVertexData(vertexBuffer);
        setStride(stride);
        setBounds(minBound, maxBound);
        setPrimitiveType(PrimitiveType::TriangleStrip);
        addAttribute(Attribute::PositionSemantic, 0, Attribute::F32Type);
        update();
    }

    QTimer m_rebuildTimer;
    QVariantList m_points;
};

#endif // TRACKGEOMETRY_H
