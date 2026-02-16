#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <QVector3D>
#include <QtMath>

namespace geometry {

inline QVector3D toCartesian(double latDeg, double lonDeg) {
    double lat = qDegreesToRadians(latDeg);
    double lon = qDegreesToRadians(lonDeg);
    return {static_cast<float>(std::cos(lat) * std::sin(lon)),
            static_cast<float>(std::sin(lat)),
            static_cast<float>(std::cos(lat) * std::cos(lon))};
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

inline QVector3D interpolateLinear(const QVector3D &origin, const QVector3D &target, float fraction) {
   return origin + (target - origin) * fraction;
}

inline float arcLengthToMiles(float arcLength) {
    static constexpr float SphereRadius = 0.5f;
    static constexpr float EarthRadiusMiles = 3958.8f;
    return (arcLength / SphereRadius) * EarthRadiusMiles;
}

} // namespace geometry

#endif // GEOMETRY_UTILS_H
