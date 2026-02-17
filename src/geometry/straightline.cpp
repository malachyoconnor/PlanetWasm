#include "straightline.h"
#include "utils.h"
#include "constants.h"
static constexpr float lineWidth = 0.2f;

StraightLine::StraightLine(QQuick3DObject *parent) : QQuick3DGeometry(parent) {
    connect(this, &StraightLine::changed, this, &StraightLine::rebuild);
    rebuild();
}

void StraightLine::rebuild() {
   clear();

   const int totalVertices = (constants::Segments + 1) * 2;
   const int stride = 3 * sizeof(float);

   QByteArray vertexBuffer(totalVertices * stride, Qt::Uninitialized);
   auto* vertexWriter = reinterpret_cast<float*>(vertexBuffer.data());

   QVector3D forwardDir = (m_end - m_start).normalized();

   // Pick a stable perpendicular direction for the line width
   QVector3D up = (std::abs(QVector3D::dotProduct(forwardDir, QVector3D(0, 1, 0))) < 0.99f)
                  ? QVector3D(0, 1, 0) : QVector3D(1, 0, 0);
   QVector3D sideDir = QVector3D::crossProduct(forwardDir, up).normalized();
   QVector3D sideOffset = sideDir * (lineWidth * 0.5f);

   for (int i = 0; i <= constants::Segments; ++i) {
      float progress = static_cast<float>(i) / constants::Segments;
      QVector3D pathPoint = geometry::interpolateLinear(m_start, m_end, progress);

      QVector3D leftEdge = pathPoint + sideOffset;
      QVector3D rightEdge = pathPoint - sideOffset;

      *vertexWriter++ = leftEdge.x();  *vertexWriter++ = leftEdge.y();  *vertexWriter++ = leftEdge.z();
      *vertexWriter++ = rightEdge.x(); *vertexWriter++ = rightEdge.y(); *vertexWriter++ = rightEdge.z();
   }

   setVertexData(vertexBuffer);
   setStride(stride);
   // setBounds(minBound, maxBound);
   setPrimitiveType(PrimitiveType::TriangleStrip);
   addAttribute(Attribute::PositionSemantic, 0, Attribute::F32Type);
   update();
}
