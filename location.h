#ifndef __LOCATION_H__
#define __LOCATION_H__

typedef struct _location
{
  double lat;
  double lon;
} location;

/**
 * Returns the distance between the two locations on the Earth's surface,
 * assuming a spherical earth with radius 6378.1 km
 */
double location_distance(const location *l1, const location *l2);
double location_distance_spherical(const location *l1, const location *l2);
double location_distance_oblate(const location *l1, const location *l2);

#endif