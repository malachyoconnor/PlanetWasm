#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <QVector3D>
#include <QtMath>

namespace geometry {

inline QVector3D toCartesian(float latDeg, float lonDeg) {
    float lat = qDegreesToRadians(latDeg);
    float lon = qDegreesToRadians(lonDeg);
    return {std::cos(lat) * std::sin(lon), std::sin(lat), std::cos(lat) * std::cos(lon)};
}

inline QVector3D interpolateSpherical(const QVector3D &origin, const QVector3D &target, float fraction) {
   float cosTheta = std::clamp(QVector3D::dotProduct(origin, target), -1.0f, 1.0f);
   float angleBetween = std::acos(cosTheta);

   if (angleBetween < 1e-6f) {
      return origin;
   }

   float inverseSinAngle = 1.0f / std::sin(angleBetween);
   float weightOrigin = std::sin((1.0f - fraction) * angleBetween) * inverseSinAngle;
   float weightTarget = std::sin(fraction * angleBetween) * inverseSinAngle;

   return origin * weightOrigin + target * weightTarget;
}

} // namespace geometry

#endif // GEOMETRY_UTILS_H
