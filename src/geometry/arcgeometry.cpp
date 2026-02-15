#include "arcgeometry.h"
#include "utils.h"
#include <limits>

static constexpr float SphereRadius = 0.5f;
static constexpr float ArcHeightFactor = 0.033f;
static constexpr int Segments = 64;
static constexpr float LineWidthScale = 0.001f;
static constexpr float MinLineWidth = 0.001f;
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

   // Points too close together produce degenerate geometry — skip
   if (angleBetween < 0.01f) {
       update();
       return;
   }

   float peakHeight = std::max(MinArcHeight, ArcHeightFactor * angleBetween);
   float lineWidth = std::max(MinLineWidth, LineWidthScale * angleBetween);
   float zoomScale = std::max(0.004f, (m_cameraZ - 0.5f) / 0.5f);
   lineWidth *= zoomScale;

   const int totalVertices = (Segments + 1) * 2;
   const int stride = 3 * sizeof(float);

   QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
   auto* vertexWriter = reinterpret_cast<float*>(vertexBuffer.data());

   QVector3D minBound(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
   QVector3D maxBound(std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest());

   for (int i = 0; i <= Segments; ++i) {
      float progress = static_cast<float>(i) / Segments;
      QVector3D pathPoint = geometry::interpolateSpherical(startPoint, endPoint, progress).normalized();

      float delta = 0.001f;
      float prevT = std::max(0.0f, progress - delta);
      float nextT = std::min(1.0f, progress + delta);
      QVector3D forwardDir = (geometry::interpolateSpherical(startPoint, endPoint, nextT) -
                              geometry::interpolateSpherical(startPoint, endPoint, prevT)).normalized();

      QVector3D sideOffset = QVector3D::crossProduct(pathPoint, forwardDir).normalized() * (lineWidth * 0.5f);

      float lift = 4.0f * progress * (1.0f - progress);
      float baseOffset = 0.000f;
      float radius = SphereRadius + baseOffset + lift * peakHeight;

      QVector3D leftEdge = (pathPoint * radius) + sideOffset;
      QVector3D rightEdge = (pathPoint * radius) - sideOffset;

      *vertexWriter++ = leftEdge.x();  *vertexWriter++ = leftEdge.y();  *vertexWriter++ = leftEdge.z();
      *vertexWriter++ = rightEdge.x(); *vertexWriter++ = rightEdge.y(); *vertexWriter++ = rightEdge.z();

      for (const auto &v : {leftEdge, rightEdge}) {
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
