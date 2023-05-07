#include "coordinateutils.h"
#include "../blaster.h"
#include "../game.h"

Point CoordinateUtils::rotate_point(
        double origin_x, double origin_y, double angle, Point point) {

    // Precalculate trigonometry for saving resources
    double s = sin(angle);
    double c = cos(angle);

    // Move point to origin for rotation
    double x_translated = point.x - origin_x;
    double y_translated = point.y - origin_y;

    // Calculate rotation
    double x_new = x_translated * c - y_translated * s + origin_x;
    double y_new = x_translated * s + y_translated * c + origin_y;

    return Point{x_new, y_new};
}

bool CoordinateUtils::check_out_of_bounds(int x, int y, int buffer) {
    return ((x + buffer) < 0 || (x - buffer) > GAME_AREA_WIDTH ||
            (y + buffer) < 0 || (y - buffer) > GAME_AREA_HEIGHT);
}

int CoordinateUtils::check_side(SDL_Point *p, SDL_Point *l0, SDL_Point *l1) {
    return ((l1->x - l0->x) * (p->y - l0->y) -
            (p->x - l0->x) * (l1->y - l0->y));
}

bool CoordinateUtils::line_intersection(
        SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d) {
    // These are derived from the general equations for lines. A point P on the
    // line seqment (A1-A2) can be written as follows: P = A1 + l(A2-A1) where l
    // is the proportional length travelled from the end of the segment. Now we
    // try to find the matching point P for two different line seqments: A1 +
    // l_a(A2-A1) = B1 + l_b(B2-B1).
    //
    // when the equation is split to x and y planes we get two equations which
    // both have the same denominator.
    double denominator =
        ((b->x - a->x) * (d->y - c->y)) - ((b->y - a->y) * (d->x - c->x));
    double numerator_a =
        ((a->y - c->y) * (d->x - c->x)) - ((a->x - c->x) * (d->y - c->y));
    double numerator_b =
        ((a->y - c->y) * (b->x - a->x)) - ((a->x - c->x) * (b->y - a->y));

    // Detect coincident lines
    if (denominator == 0.0f) {
        return numerator_a == 0.0f && numerator_b == 0.0f;
    }

    double l_a = numerator_a / denominator;
    double l_b = numerator_b / denominator;

    // If there is an intersection point WITHIN the range of the segments (0,1)
    // a collision is detected.
    return ((l_a >= 0.0f && l_a <= 1.0f) && (l_b >= 0.0f && l_b <= 1.0f));
}

bool CoordinateUtils::line_intersection_point(SDL_Point *a1, SDL_Point *a2,
                                            SDL_Point *b1, SDL_Point *b2,
                                            SDL_Point *out) {
    // Form lines from homogeneous coordinates.
    // A cartesian point (x,y) in homogeneous form is (x,y,1)
    // A line interesecting two points can be calculated in homogenous
    // coordinates as a cross-product: l1 = cross(x1, x2) Similarly, the point
    // of intersection between two lines can be written as p = cross(l1, l2) =
    // (x,y,w) Which can be converted to cartesian: (x/w, y/w) If w = 0, the
    // lines don't intersect

    // Cartesian to homogeneous
    HomogeneousPoint a1_h = {static_cast<double>(a1->x),
                             static_cast<double>(a1->y), 1};
    HomogeneousPoint a2_h = {static_cast<double>(a2->x),
                             static_cast<double>(a2->y), 1};
    HomogeneousPoint b1_h = {static_cast<double>(b1->x),
                             static_cast<double>(b1->y), 1};
    HomogeneousPoint b2_h = {static_cast<double>(b2->x),
                             static_cast<double>(b2->y), 1};

    // Form lines and find point of intersection
    auto l1 = CoordinateUtils::crossProduct(a1_h, a2_h);
    auto l2 = CoordinateUtils::crossProduct(b1_h, b2_h);
    auto intersection = CoordinateUtils::crossProduct(l1, l2);

    // Check if a point of intersection is found
    if (intersection.w == 0.0f) {
        out->x = 0;
        out->y = 0;
        return false;
    } else {
        int x = static_cast<int>(intersection.x / intersection.w);
        int y = static_cast<int>(intersection.y / intersection.w);
        // TODO: Maybe some sanity checks here?
        out->x = x;
        out->y = y;
        return true;
    }
}

HomogeneousPoint CoordinateUtils::crossProduct(const HomogeneousPoint &a,
                                             const HomogeneousPoint &b) {
    HomogeneousPoint ret = {a.y * b.w - b.y * a.w,
                            a.w * b.x - b.w * a.x,
                            a.x * b.y - b.x * a.y};
    return ret;
}

void CoordinateUtils::translate(SDL_Point *dest, SDL_Point *src,
                              int size, int offset_x, int offset_y) {
    for (int i = 0; i < size; i++) {
        dest[i].x = src[i].x + offset_x;
        dest[i].y = src[i].y + offset_y;
    }
}
