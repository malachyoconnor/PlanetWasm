#include "arcgeometry.h"
#include "utils.h"

static constexpr float ArcRadius = 151.0f;
static constexpr int Segments = 64;
static constexpr float LineWidth = 1.0f;

ArcGeometry::ArcGeometry(QQuick3DObject *parent) : QQuick3DGeometry(parent) {
    connect(this, &ArcGeometry::changed, this, &ArcGeometry::rebuild);
    rebuild();
}

void ArcGeometry::rebuild() {
   clear();

   const QVector3D startPoint = geometry::toCartesian(m_startLat, m_startLon).normalized();
   const QVector3D endPoint = geometry::toCartesian(m_endLat, m_endLon).normalized();

   const int totalVertices = (Segments + 1) * 2;
   const int stride = 3 * sizeof(float);

   QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
   auto* vertexWriter = reinterpret_cast<float*>(vertexBuffer.data());

   for (int i = 0; i <= Segments; ++i) {
      float progress = static_cast<float>(i) / Segments;
      QVector3D pathPoint = geometry::interpolateSpherical(startPoint, endPoint, progress).normalized();

      float delta = 0.001f;
      float prevT = std::max(0.0f, progress - delta);
      float nextT = std::min(1.0f, progress + delta);

      QVector3D forwardDir = (geometry::interpolateSpherical(startPoint, endPoint, nextT) -
                              geometry::interpolateSpherical(startPoint, endPoint, prevT)).normalized();

      QVector3D sideOffset = QVector3D::crossProduct(pathPoint, forwardDir).normalized() * (LineWidth * 0.5f);

      // Both offsets from the center of this new path point
      QVector3D leftEdge = (pathPoint * ArcRadius) + sideOffset;
      QVector3D rightEdge = (pathPoint * ArcRadius) - sideOffset;

      // Add these edges to be drawn - we draw a triangle strip.
      // Meaning we "zip up" these points into a load of triangles.
      *vertexWriter++ = leftEdge.x();  *vertexWriter++ = leftEdge.y();  *vertexWriter++ = leftEdge.z();
      *vertexWriter++ = rightEdge.x(); *vertexWriter++ = rightEdge.y(); *vertexWriter++ = rightEdge.z();
   }

   setVertexData(vertexBuffer);
   setStride(stride);
   setBounds({-ArcRadius, -ArcRadius, -ArcRadius}, {ArcRadius, ArcRadius, ArcRadius});
   setPrimitiveType(PrimitiveType::TriangleStrip);
   addAttribute(Attribute::PositionSemantic, 0, Attribute::F32Type);
   update();
}
