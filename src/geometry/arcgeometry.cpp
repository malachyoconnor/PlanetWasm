#include "arcgeometry.h"
#include "utils.h"
#include <limits>

static constexpr float SphereRadius = 0.5f;
static constexpr float ArcHeightFactor = 0.033f;
static constexpr int Segments = 64;
static constexpr float LineWidthScale = 0.0001f;
static constexpr float MinLineWidth = 0.0005f;
static constexpr float MinArcHeight = 0.0f;

ArcGeometry::ArcGeometry(QQuick3DObject *parent) : QQuick3DGeometry(parent) {
    m_rebuildTimer.setSingleShot(true);
    m_rebuildTimer.setInterval(50);
    connect(&m_rebuildTimer, &QTimer::timeout, this, &ArcGeometry::rebuild);
    connect(this, &ArcGeometry::changed, &m_rebuildTimer, qOverload<>(&QTimer::start));
    rebuild();
}

void ArcGeometry::rebuild() {
   clear();

   const QVector3D startPoint = geometry::toCartesian(m_startLat, m_startLon).normalized();
   const QVector3D endPoint = geometry::toCartesian(m_endLat, m_endLon).normalized();

   float cosTheta = std::clamp(QVector3D::dotProduct(startPoint, endPoint), -1.0f, 1.0f);
   float angleBetween = std::acos(cosTheta);
   float peakHeight = std::max(MinArcHeight, ArcHeightFactor * angleBetween);
   float lineWidth = std::max(MinLineWidth, LineWidthScale * angleBetween);
   float zoomScale = std::max(0.004f, (m_cameraZ - 0.5f) / 0.5f);
   lineWidth *= zoomScale;


   // For very close points, the per-segment numerical derivative of the slerp is
   // below float precision, producing a degenerate forward direction. Precompute
   // a stable forward direction to use as fallback.
   QVector3D stableForward;
   const bool useStableForward = (angleBetween < 0.01f);
   if (useStableForward) {
       QVector3D diff = endPoint - startPoint;
       if (diff.lengthSquared() > 1e-12f) {
           stableForward = diff.normalized();
       } else {
           // Exactly coincident — pick an arbitrary tangent perpendicular to the point
           stableForward = QVector3D::crossProduct(startPoint, QVector3D(0, 1, 0));
           if (stableForward.lengthSquared() < 1e-6f)
               stableForward = QVector3D::crossProduct(startPoint, QVector3D(1, 0, 0));
           stableForward.normalize();
       }
   }

   const int totalVertices = (Segments + 1) * 2;
   const int stride = 3 * sizeof(float);

   QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
   auto* vertexWriter = reinterpret_cast<float*>(vertexBuffer.data());

   for (int i = 0; i <= Segments; ++i) {
      float progress = static_cast<float>(i) / Segments;
      QVector3D pathPoint = geometry::interpolateSpherical(startPoint, endPoint, progress).normalized();

      QVector3D forwardDir;
      if (useStableForward) {
          forwardDir = stableForward;
      } else {
          float delta = 0.001f;
          float prevT = std::max(0.0f, progress - delta);
          float nextT = std::min(1.0f, progress + delta);
          forwardDir = (geometry::interpolateSpherical(startPoint, endPoint, nextT) -
                        geometry::interpolateSpherical(startPoint, endPoint, prevT)).normalized();
      }

      QVector3D sideOffset = QVector3D::crossProduct(pathPoint, forwardDir).normalized() * (lineWidth * 0.5f);

      float lift = 4.0f * progress * (1.0f - progress);
      float baseOffset = 0.001f;
      float radius = SphereRadius + baseOffset + lift * peakHeight;

      QVector3D leftEdge = (pathPoint * radius) + sideOffset;
      QVector3D rightEdge = (pathPoint * radius) - sideOffset;

      // Add these edges to be drawn - we draw a triangle strip.
      // Meaning we "zip up" these points into a load of triangles.
      *vertexWriter++ = leftEdge.x();  *vertexWriter++ = leftEdge.y();  *vertexWriter++ = leftEdge.z();
      *vertexWriter++ = rightEdge.x(); *vertexWriter++ = rightEdge.y(); *vertexWriter++ = rightEdge.z();
   }

   QVector3D minBound(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
   QVector3D maxBound(std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest());

   const float* reader = reinterpret_cast<const float*>(vertexBuffer.constData());
   for (int i = 0; i < totalVertices; ++i) {
       float x = reader[i * 3 + 0];
       float y = reader[i * 3 + 1];
       float z = reader[i * 3 + 2];
       minBound = QVector3D(std::min(minBound.x(), x), std::min(minBound.y(), y), std::min(minBound.z(), z));
       maxBound = QVector3D(std::max(maxBound.x(), x), std::max(maxBound.y(), y), std::max(maxBound.z(), z));
   }

   setVertexData(vertexBuffer);
   setStride(stride);
   setBounds(minBound, maxBound);
   setPrimitiveType(PrimitiveType::TriangleStrip);
   addAttribute(Attribute::PositionSemantic, 0, Attribute::F32Type);
   update();
}
