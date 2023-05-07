#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../blaster.h"
#include "../rendering/renderobject.h"
#include "SDL2/SDL.h"
#include "coordinateutils.h"
#include <memory>
#include <renderengine.h>
#include <stdexcept>
#include <vector>

class RenderEngine;
enum ScreenPosition { NE, SE, SW, NW };
enum Dimension { X, Y };
enum Extremity { MIN, MAX };

///
/// \brief The RenderType enum defines the rendering style.
///
/// Primitives rendered as LINE will have connecting lines between outline
/// points.
///
enum RenderType { FILL, LINE, POINT };

///
/// \brief Class representing any 2D graphics entity
///
/// SDL coordinates and positions are handled as integers and are such unusable
/// for movement and rotational calculations due to possible cumulative rounding
/// errors. To overcome this, all graphic instances store their coordinates as
/// double values and all calculations are done using these numbers.
///
/// After each calculation updateOutline_() is called to update the SDL
/// -compatible integer coordinates from the newly refreshed double values.
///
/// Important variables:
///
/// SDL_Point* outline      Holds the integer based coordinates for rendering
/// Coordinate coordinates  Hold the double valued coordinates for calculations
///
class Polygon : public RenderObject {

  public:
    typedef std::shared_ptr<Polygon> Ptr;

  private:
    ///
    /// \brief Finds the position of an objects extreme point
    /// in the given direction.
    /// \param d dimension to search in
    /// \param e extremity to find
    /// \return extreme point position
    ///
    int getExtremePoint_(Dimension d, Extremity e) const;

    ///
    /// \brief Searches extreme point locations
    ///
    void searchExtremes_();

    ///
    /// \brief Updates extreme point locations
    ///
    void updateExtremes_();

    ///
    /// \brief Updates the objects center point based
    ///  on internal floating point values.
    ///
    void updateCenterPoint_();

    ///
    /// \brief Updates the objects outline coordinates
    /// based on internal floating point values.
    ///
    void updateOutline_();

    // Variables
    double max_r_;
    double x_, y_;
    int max_x_, min_x_, max_y_, min_y_;
    double max_x_d_, min_x_d_, max_y_d_, min_y_d_;
    bool maxValUpdated_ = false;
    RenderType renderType_ = LINE;
    SDL_Color color_ = SDL_Color{0xff, 0xff, 0xff, 0xff};
    SDL_Point *render_buffer = nullptr;

  public:
    int x, y;
    SDL_Point *outline = nullptr;
    std::vector<Point> coordinates;
    int points;

    ///
    /// Note that the outline array is freed on destruction of this object.
    /// If the outline was created outside of this class and passed on as a
    /// pointer, care should be taken not to reference the initial outline after
    /// the lifetime of this instance.
    ///
    ~Polygon() override;

    Polygon();

    ///
    /// \brief Constructs a 2D graphics primitive consisting of
    /// line segments
    /// \param outline pointer to outline array
    /// \param points number of points on the primitive
    /// (endpoints are counted in also)
    /// \param x initial center x coordinate
    /// \param y initial center y coordinate
    ///
    Polygon(RenderEngine *renderEngine, SDL_Point *outline, int points,
                int x, int y);

    ///
    /// \brief Constructs a 2D graphics primitive consisting of
    /// line segments
    /// \param outline vector holding the outline point
    /// coordinates
    /// \param x initial center x coordinate
    /// \param y initial center y coordinate
    ///
    Polygon(RenderEngine *renderEngine, std::vector<SDL_Point> outline,
                int x, int y);

    void init(RenderEngine *renderEngine, std::vector<SDL_Point> initial_outline,
              int x, int y);

    ///
    /// \brief Moves each point of the primitive by given amount of
    /// pixels
    /// \param amount_x x movement
    /// \param amount_y y movement
    ///
    void move(double amount_x, double amount_y);

    ///
    /// \brief Moves each point of the primitive to absolute
    /// location (by center point)
    /// \param dest_x x destination
    /// \param dest_y y destination
    ///
    void moveAbsolute(double dest_x, double dest_y);

    ///
    /// \brief Rotates the primitive by given angle around given
    /// point
    /// \param angle_rad amount of rotation in radians
    /// \param x rotation origin x
    /// \param y rotation origin y
    ///
    void rotate(double angle_rad, int x, int y);

    ///
    /// \brief Checks if the primitive intersects another one
    /// \param p pointer to another primitive
    /// \return true if the pirmitives intersect
    ///
    bool intersects(Polygon *p);

    ///
    /// \brief Checks if the primitive is completely out of bounds (OOB).
    /// \param buffer optional buffer in pixels. Individual points have to
    /// be OOB at least by this amount.
    /// \return true if the primitive is OOB
    ///
    bool outOfBounds(int buffer = 0);

    /// Checks if primitives are close to each other as a coarse estimate
    /// \param p pointer to another graphics primitive
    /// \return true if the primitives are near each other
    bool isCloseToQ(Polygon *p);

    ///
    /// \brief Checks if the two primitices are within the same X/Y
    /// range based on their extreme points.
    /// \param p pointer to another graphics primitive
    /// \return true if the primitives are within range of each other,
    /// meaning that it is possible for them to intersect.
    ///
    bool isCloseTo(Polygon *p);

    ///
    /// \brief Checks if the given point lies between the
    /// extremities of the primitive
    /// \param p pointer to the location to check against
    /// \return true if the point is within range of
    /// the primitives extreme points.
    ///
    bool isCloseTo(SDL_Point *p);

    ///
    /// \brief Rotates the polygon by given angle around the center
    /// point
    /// \param andle_rad angle (in radians) to rotate
    ///
    void rotate(double andle_rad);

    ///
    /// \brief Checks if the polygon contains (encloses) given
    /// point
    /// \param p target point
    /// \return true if point is within polygon
    ///
    bool contains(SDL_Point *p) const;

    ///
    /// \brief Gets the current screen position of the
    /// center point
    /// \return current screen quarter of the
    /// polygon
    ///
    [[nodiscard]] ScreenPosition getScreenPosition() const;

    ///
    /// \brief Sets color used in rendering
    /// \param color render color
    ///
    void setColor(SDL_Color color);

    ///
    /// \brief Sets graphics primitive render type; whether to
    /// render connecting lines between outline points or not.
    /// \param renderType rendering type.
    ///
    void setRenderType(RenderType renderType);

    ///
    /// \brief Renders the graphics primitive with the given camera
    /// offset.
    /// \param offset_x x offset
    /// \param offset_y y offset
    ///
    void render(int offset_x, int offset_y) override;

    /// Getter methods for extreme points of the primitive
    int getMaxX();
    int getMinX();
    int getMaxY();
    int getMinY();
    [[nodiscard]] double getMaxRadius() const;

    ///
    /// \brief Returns the render color
    /// \return color render color
    ///
    SDL_Color getColor();

  private:
    template <typename T> bool checkClosedOutline_(T initial_outline) {
        if ((initial_outline[0].x == initial_outline[points - 1].x) &&
            (initial_outline[0].y == initial_outline[points - 1].y)) {
            return true;
        } else {
            return false;
        }
    }

    void calculateFill_();
    void moveFill_(int x_amount, int y_amount);
    SDL_Point* fill_render_buffer = nullptr;

    /// Class variables
    SDL_Point *fill = nullptr;
    int n_fill_points = 0;

};

#endif // GRAPHICS_H
