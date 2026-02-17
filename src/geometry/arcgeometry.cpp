#include "arcgeometry.h"
#include "utils.h"
#include "constants.h"
#include <limits>

static constexpr float LineWidthScale = 0.001f;
static constexpr float MinLineWidth = 0.002f;
static constexpr float MaxArcHeight = 0.01f;
static constexpr float LateralOffset = 0.002f;

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

   float distanceInMiles = constants::MilesPerDistance * (endPoint - startPoint).length();
   // Clamp to between 300 and 600 km
   float heightMultiplier = std::clamp(distanceInMiles, 300.0f , 600.0f) / 600.0f;
   float peakHeight = heightMultiplier * MaxArcHeight;

   // Points too close together produce degenerate geometry — skip
   if (angleBetween < 0.01f) {
       update();
       return;
   }

   float lineWidth = std::max(MinLineWidth, LineWidthScale * angleBetween);
   float zoomScale = std::max(0.004f, (m_cameraZ - 0.5f) / 0.5f);
   lineWidth *= zoomScale;

   const int totalVertices = (constants::Segments + 1) * 2;
   const int stride = 5 * sizeof(float);

   QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
   auto* vertexWriter = reinterpret_cast<float*>(vertexBuffer.data());

   QVector3D minBound(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
   QVector3D maxBound(std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest());

   for (int i = 0; i <= constants::Segments; ++i) {
      float progress = static_cast<float>(i) / constants::Segments;
      QVector3D pathPoint = geometry::interpolateSpherical(startPoint, endPoint, progress).normalized();

      float delta = 0.001f;
      float prevT = std::max(0.0f, progress - delta);
      float nextT = std::min(1.0f, progress + delta);
      QVector3D forwardDir = (geometry::interpolateSpherical(startPoint, endPoint, nextT) -
                              geometry::interpolateSpherical(startPoint, endPoint, prevT)).normalized();

      QVector3D sideDir = QVector3D::crossProduct(pathPoint, forwardDir).normalized();
      QVector3D sideOffset = sideDir * (lineWidth * 0.5f);

      float lift = 4.0f * progress * (1.0f - progress);
      float radius = constants::SphereRadius + lift * peakHeight;

      QVector3D lateralShift = sideDir * (LateralOffset * lift);
      QVector3D center = pathPoint * radius + lateralShift;
      QVector3D leftEdge = center + sideOffset;
      QVector3D rightEdge = center - sideOffset;

      *vertexWriter++ = leftEdge.x();  *vertexWriter++ = leftEdge.y();  *vertexWriter++ = leftEdge.z();
      *vertexWriter++ = progress;      *vertexWriter++ = 0.0f;
      *vertexWriter++ = rightEdge.x(); *vertexWriter++ = rightEdge.y(); *vertexWriter++ = rightEdge.z();
      *vertexWriter++ = progress;      *vertexWriter++ = 1.0f;

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
   addAttribute(Attribute::TexCoord0Semantic, 3 * sizeof(float), Attribute::F32Type);
   update();
}
