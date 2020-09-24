#include <stddef.h>
#include <math.h>
#include <stdio.h>


#include "location.h"

#define EARTH_RADIUS_KM 6371
#define SEMI_MAJOR 6378.137
#define FLATTENING (1.0 / 298.257223563)
#define SEMI_MINOR ((1.0 - FLATTENING) * SEMI_MAJOR)


#define PI 3.14159265358979
#define RADIANS(x) ((x) / 180.0 * PI)
#define ABSD(x) ((x) >= 0 ? (x) : -(x))

/**
 * Determines if the given location is valid.  A location is valid if the
 * latitude is between -90 and 90 degrees.
 */
int location_validate(const location *l)
{
  if (l != NULL && isfinite(l->lat) && l->lat >= -90.0 && l->lat <= 90.0 && isfinite(l->lon))
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

/**
 * Returns the distance between the two locations on the Earth's surface,
 * assuming a spherical earth with radius 6371 km.  If either location
 * is invalid, the return value is NaN.
 */
double location_distance(const location *l1, const location *l2)
{
  return location_distance_oblate(l1, l2);
}

double location_distance_spherical(const location *l1, const location *l2)
{
  if (location_validate(l1) && location_validate(l2))
    {
      double delta_lon = RADIANS(l1->lon - l2->lon);
      double colat1 = 90.0 - l1->lat;
      double colat2 = 90.0 - l2->lat;
      colat1 = RADIANS(colat1);
      colat2 = RADIANS(colat2);
      double angle = acos(cos(colat1) * cos(colat2)
                          + sin(colat1) * sin(colat2) * cos(delta_lon));
      return EARTH_RADIUS_KM * angle;
    }
  else
    {
      return nan("");
    }
}


// from https://www.movable-type.co.uk/scripts/latlong-vincenty.html
double location_distance_oblate(const location *l1, const location *l2)
{
  if (!location_validate(l1) || !location_validate(l2))
    {
      return nan("");
    }
  else if (l1->lat == l2->lat && (l1->lat == -90.0 || l1->lat == 90.0 || l1->lon == l2->lon))
    {
      return 0.0;
    }

  double L = RADIANS(l2->lon - l1->lon);

  double tanU1 = (1 - FLATTENING) * tan(RADIANS(l1->lat));
  double cosU1 = 1 / sqrt((1 + tanU1 * tanU1));
  double sinU1 = tanU1 * cosU1;

  double tanU2 = (1 - FLATTENING) * tan(RADIANS(l2->lat));
  double cosU2 = 1 / sqrt((1 + tanU2 * tanU2));
  double sinU2 = tanU2 * cosU2;

  double lambda = L;
  double last_lambda;
  int iterations_left = 100;

  double cos_sq_alpha;
  double cos_2sigmam;
  double sin_sig;
  double cos_sig;
  double sigma;

  do {
    double sin_lam = sin(lambda);
    double cos_lam = cos(lambda);
    double sin_sq_sig = (cosU2 * sin_lam) * (cosU2 * sin_lam) + (cosU1 * sinU2 - sinU1 * cosU2 * cos_lam) * (cosU1 * sinU2 - sinU1 * cosU2 * cos_lam);
    sin_sig = sqrt(sin_sq_sig);

    if (sin_sig == 0) return 0;  // co-incident points

    cos_sig = sinU1 * sinU2 + cosU1 * cosU2 * cos_lam;
    sigma = atan2(sin_sig, cos_sig);
    double sin_alpha = cosU1 * cosU2 * sin_lam / sin_sig;
    cos_sq_alpha = 1 - pow(sin_alpha, 2);
    cos_2sigmam = cos_sig - 2* sinU1 * sinU2 / cos_sq_alpha;

    if (isnan(cos_2sigmam))
      {
        cos_2sigmam = 0;  // equatorial line
      }

    double C = FLATTENING / 16 * cos_sq_alpha * (4 + FLATTENING * (4 - 3 * cos_sq_alpha));
    last_lambda = lambda;
    lambda = L + (1 - C) * FLATTENING * sin_alpha * (sigma + C * sin_sig * (cos_2sigmam + C * cos_sig * (-1 + 2 * pow(cos_2sigmam, 2))));

  } while (ABSD(lambda - last_lambda) > 1e-12 && --iterations_left > 0);

  if (iterations_left == 0)
    {
      return nan("");
    }

  double uSq = cos_sq_alpha * (pow(SEMI_MAJOR, 2) - pow(SEMI_MINOR, 2)) / pow(SEMI_MINOR, 2);
  double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
  double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
  double delta_sig = B * sin_sig * (cos_2sigmam + B / 4 * (cos_sig * (-1 + 2 * pow(cos_2sigmam ,2)) - B / 6 * cos_2sigmam * (-3 + 4 * pow(sin_sig, 2)) * (-3 + 4 * pow(cos_2sigmam, 2))));

  return SEMI_MINOR * A *(sigma - delta_sig);
}