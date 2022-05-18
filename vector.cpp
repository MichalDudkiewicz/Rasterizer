#include "vector.hpp"

float3 crossProduct(const float3& firstVector, const float3& secondVector)
{
return {firstVector.y() * secondVector.z() - firstVector.z() * secondVector.y(), firstVector.z() * secondVector.x() - firstVector.x() * secondVector.z(), firstVector.x() * secondVector.y() - firstVector.y() * secondVector.x()};
}

