#ifndef PLANETWASM_CONSTANTS_H
#define PLANETWASM_CONSTANTS_H

namespace constants {

inline constexpr float SphereRadius = 0.5f;
inline constexpr float EarthRadiusMiles = 3958.8f;
inline constexpr float MilesPerDistance = EarthRadiusMiles / SphereRadius;
inline constexpr int Segments = 64;
inline constexpr float FlightDistanceMiles = 300.0f;
inline constexpr int DirectionChangeDegrees = 30;

} // namespace constants

#endif // PLANETWASM_CONSTANTS_H
