#ifndef UTILITY_H
#define UTILITY_H

namespace conf {

//Conversion factors for pixels to MKS
static constexpr float ppm = 15.f;            //Pixels-Per-Meter
static constexpr float mpp = 1.f/ppm;         //Meters-Per-Pixel

//Dimentions of spawned entities
static constexpr float box_halfwidth = 20.f * mpp;  //Halfwidth of spwaned boxed in px
static constexpr float circle_radius = 30.f * mpp;  //Radius of spawned circles in px

}

//Convert a pixels value to meters
float meters(int pixels);

//Do the oppsite
float pixels(long double meters);

//Allows literals like 10px, which convert to the meter value
float operator"" _px(unsigned long long int _pixels);

#endif // UTILITY_H
