#include "angle.h"
#include <cmath>

double find_angle_between_vectors(const std::vector<double>& vector1, const std::vector<double>& vector2) {
    // Calculate the dot product between the vectors
    double dot_product = vector1[0] * vector2[0] + vector1[1] * vector2[1];

    // Calculate the magnitudes of the vectors
    double magnitude1 = sqrt(vector1[0] * vector1[0] + vector1[1] * vector1[1]);
    double magnitude2 = sqrt(vector2[0] * vector2[0] + vector2[1] * vector2[1]);

    // Calculate the cosine of the angle between the vectors
    double cos_angle = dot_product / (magnitude1 * magnitude2);

    // Calculate the angle in radians
    double angle_radians = acos(cos_angle);

    // Convert the angle to degrees
    double angle_degrees = angle_radians * 180.0 / M_PI;

    return angle_degrees;
}