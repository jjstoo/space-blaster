#ifndef TEXTENGINE_H
#define TEXTENGINE_H

#include "../rendering/renderobject.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <memory>
#include <string>

class TextEngine : public RenderObject {
  public:
    typedef std::shared_ptr<TextEngine> Ptr;

  public:
    ///
    /// \brief Class for rendering text content
    /// \param renderEngine RenderEngine instance
    ///
    TextEngine(RenderEngine *renderEngine);

    ///
    /// \brief Class for rendering text content
    /// \param renderEngine RenderEngine instance
    /// \param xpos text position x
    /// \param ypos text position y
    /// \param font_size font size
    ///
    TextEngine(RenderEngine *renderEngine, int xpos, int ypos, int font_size);
    ~TextEngine();

    ///
    /// \brief Sets text position
    /// \param xpos text position x
    /// \param ypos text position y
    ///
    void setPosition(int xpos, int ypos);

    ///
    /// \brief Sets font size
    /// \param size font size
    ///
    void setFontSize(int size);

    ///
    /// \brief Sets text color
    /// \param r red
    /// \param b blue
    /// \param g green
    /// \param a alpha
    ///
    void setColor(Uint8 r, Uint8 b, Uint8 g, Uint8 a);

    ///
    /// \brief Sets text content
    /// \param text string to display
    ///
    void setText(std::string text);

    ///
    /// \brief Constructs a progress bar
    /// \param progress current progress
    /// \param max progress max value
    /// \param width bar width (in symbols)
    /// \param prefix bar title
    ///
    void setProgressBar(int progress, int max, int width,
                        std::string prefix = "", std::string postfix = "");

    ///
    /// \brief Renders current text content
    ///
    void render(int offset_x, int offset_y) override;

  private:
    SDL_Surface *surface_ = nullptr;
    SDL_Texture *texture_ = nullptr;
    SDL_Color color_;
    TTF_Font *font_ = nullptr;
    SDL_Rect position_;
    SDL_RWops *io_;

    std::string text_;

    // Position
    int x_;
    int y_;

    // Color
    Uint8 r_;
    Uint8 b_;
    Uint8 g_;
    Uint8 a_;
};

#endif // TEXTENGINE_H
