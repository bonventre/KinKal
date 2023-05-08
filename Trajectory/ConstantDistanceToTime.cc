#include "KinKal/Trajectory/ConstantDistanceToTime.hh"
#include <cstdlib>

ConstantDistanceToTime::ConstantDistanceToTime(double constantSpeed, double timeOffset) :
  DistanceToTime(timeOffset), constantSpeed_(constantSpeed) {}

double ConstantDistanceToTime::distance(double deltaT) {
    return (deltaT - timeOffset_) * constantSpeed_;
}

double ConstantDistanceToTime::time(double distance) {
    return distance * inverseSpeed(distance) + timeOffset_;
}

double ConstantDistanceToTime::speed(double distance) {
    return constantSpeed_;
}

double ConstantDistanceToTime::inverseSpeed(double distance) {
    double static const speedOfLight = 299792458.0; 
    if (abs(constantSpeed_) < 1/speedOfLight) {
        return speedOfLight;
    }
    return 1/constantSpeed_;
}