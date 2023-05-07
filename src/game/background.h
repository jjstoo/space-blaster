#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <vector>

#include "SDL2/SDL.h"

#include "../rendering/renderengine.h"
#include "../rendering/renderobject.h"
#include "graphics.h"

const static double stars_s_density = 0.001;
const static double stars_m_density = 0.0001;
const static double stars_l_density = 0.00001;

enum star_types { STAR_S, STAR_M, STAR_L };

///
/// \brief The Background class represents a static background of stars and
/// planets
///
class Background {
  public:
    ~Background();
    Background(RenderEngine *renderEngine);

  private:
    // Graphics elements
    Polygon *stars_s_;
    Polygon *stars_m_;
    Polygon *stars_l_;

    // Functions

    ///
    /// \brief Creates a background vector filled with
    /// stars of the given type.
    /// \param type star type
    /// \param density star field density
    /// \param w spawn area width
    /// \param h spawn area height
    /// \return vector with SDL_Point points belonging to the generated stars
    ///
    std::vector<SDL_Point> createBackground_(star_types type, double density,
                                             int w, int h);

    ///
    /// \brief Returns SDL_Point points belonging to a star by its type
    /// and center point
    /// \param type star type
    /// \param x center x coordinate
    /// \param y center y coordinate
    /// \return vector with SDL_Point points belonging to the star
    ///
    std::vector<SDL_Point> getStarShape_(star_types type, int x, int y);
};

#endif // BACKGROUND_H
