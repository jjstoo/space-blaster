#ifndef COORDINATEUTILS_H
#define COORDINATEUTILS_H

#include "SDL2/SDL.h"
#include <math.h>

typedef struct point {
    double x;
    double y;
} Point;

typedef struct HomogeneousPoint {
    double x;
    double y;
    double w;
} HomogeneousPoint;


///
/// \brief Tools for graphics operation and coordinate transforms
///
namespace CoordinateUtils {

    template <typename PointType>
    extern inline double distance(PointType a, PointType b) {
        auto xd = a.x - b.x;
        auto yd = a.y - b.y;
        return std::sqrt(xd*xd + yd*yd);
    }

    template <typename PointType>
    extern inline double l1distance(PointType a, PointType b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    template <typename PointType>
    extern inline double angle_between(PointType a, PointType b) {
        return atan2(b.y - a.y, b.x - a.x);
    }

    ///
    /// \brief Checks if given point is out of bounds by at
    /// least with the given amount of buffer.
    /// \param x x coordinate
    /// \param y y coordinate
    /// \param buffer Amount of buffer pixels required for the point to
    /// be considered out of bounds.
    /// \return true if the point is out of bounds
    ///
    extern bool check_out_of_bounds(int x, int y, int buffer = 0);

    ///
    /// \brief Rotates given point around specified origin
    /// \param origin_x origin position x
    /// \param origin_y origin position y
    /// \param angle angle (in radians) to rotate
    /// \param point target point
    /// \return Rotated point
    ///
    extern Point rotate_point(double origin_x, double origin_y, double angle,
                              Point point);

    ///
    /// \brief Checks if the poin p lies left|on|right of a line
    /// defined by l0 and l1
    /// \param p  point to check
    /// \param l0 point on line 0
    /// \param l1 point on lin 1
    /// \return             > 0 for p on left side
    ///                     = 0 for p on line
    ///                     < 0 for p on right side
    ///
    extern int check_side(SDL_Point *p, SDL_Point *l0, SDL_Point *l1);

    ///
    /// \brief Checks if two line segments defined by the
    /// coordinates cross
    /// \param a line segment 1 start
    /// \param b line segment 1 end
    /// \param c line segment 2 start
    /// \param d line segment 2 end
    /// \return
    ///
    extern bool line_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c,
                                  SDL_Point *d);

    ///
    /// \brief Find the point of interesection of two points
    /// \param x1 first point on line 1
    /// \param x2 second point on line 1
    /// \param y1 first point on line 2
    /// \param y2 second point on line 2
    /// \param out point of interesection
    /// \return boolean indicating if the lines intersect
    ///
    extern bool line_intersection_point(SDL_Point *a1, SDL_Point *a2,
                                        SDL_Point *b1, SDL_Point *b2,
                                        SDL_Point *out);

    ///
    /// \brief Calculate cross product between points
    /// \param a point a
    /// \param b point b
    /// \return cross product
    ///
    extern HomogeneousPoint crossProduct(const HomogeneousPoint &a,
                                         const HomogeneousPoint &b);

    ///
    /// \brief Translates point(s) by given offset values
    /// \param dest destination pointer
    /// \param src source pointer
    /// \param size length of the point array
    /// \param offset_x x offset
    /// \param offset_y y offset
    /// \return translated point
    ///
    extern void translate(SDL_Point *dest, SDL_Point *src,
                          int size, int offset_x, int offset_y);
};

#endif // COORDINATEUTILS_H
