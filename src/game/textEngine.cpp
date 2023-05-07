#include "textEngine.h"
#include "../game.h"
#include "../resources/FreeSans.h"

TextEngine::TextEngine(RenderEngine *renderEngine)
    : RenderObject(renderEngine), r_(255), b_(255), g_(255), a_(255) {}

TextEngine::TextEngine(RenderEngine *renderEngine,
                       int xpos, int ypos, int font_size)
    : RenderObject(renderEngine), r_(255), b_(255), g_(255), a_(255) {
    setFontSize(font_size);
    setPosition(xpos, ypos);
}

TextEngine::~TextEngine() {
    TTF_CloseFont(font_);
    SDL_DestroyTexture(texture_);
    SDL_FreeSurface(surface_);
    SDL_RWclose(io_);
}

void TextEngine::setFontSize(int size) {
    io_ = SDL_RWFromMem(FreeSans::getBytes(), FreeSans::getSize());
    font_ = TTF_OpenFontRW(io_, 0, size);

    if (font_ == nullptr) {
        LOG("Font loading failed");
    }
}

void TextEngine::setColor(Uint8 r, Uint8 b, Uint8 g, Uint8 a) {
    r_ = r;
    b_ = b;
    g_ = g;
    a_ = a;
}

void TextEngine::setPosition(int xpos, int ypos) {
    x_ = xpos;
    y_ = ypos;
}

void TextEngine::setText(std::string text) {

    // Stop if the new text does not bring any changes
    if (text_ == text)
        return;

    SDL_Color color = {r_, g_, b_, // color
                       a_};        // alpha

    SDL_FreeSurface(surface_);
    SDL_DestroyTexture(texture_);

    surface_ = TTF_RenderText_Solid(font_, text.c_str(), color);
    texture_ = SDL_CreateTextureFromSurface(Game::RENDERER, surface_);

    text_ = text;
}

void TextEngine::setProgressBar(int progress, int max, int width,
                                std::string prefix, std::string postfix) {
    std::string out = "[";
    int cur_idx = (progress / max) * width;
    for (int i = 0; i < width; ++i) {
        if (i < cur_idx)
            out += "|";
        else
            out += " ";
    }

    out += "]";
    setText(prefix + " " + out + " " + postfix);
}

void TextEngine::render(int offset_x, int offset_y) {
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture_, nullptr, nullptr, &texW, &texH);
    position_ = {x_, y_, texW, texH};
    SDL_RenderCopy(Game::RENDERER, texture_, nullptr, &position_);
}
