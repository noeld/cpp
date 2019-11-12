// main.cpp
#include "SDL.h"
#include "SDL_image.h"
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>

using window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
using texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using surface_ptr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;

texture_ptr load_texture(char const * filename, SDL_Renderer* renderer)
{
    surface_ptr surface
    (
        IMG_Load(filename)
        , &SDL_FreeSurface
    );
    if (!surface)
        throw "Cannot load surface)";
    texture_ptr texture
    (
        SDL_CreateTextureFromSurface(renderer, surface.get())
        , &SDL_DestroyTexture
    );
    return texture;
}

const int SCREEN_WIDTH  =1280; 
const int SCREEN_HEIGHT = 800;

int main(int argc, char const *argv[])
{
    int ret = 0;
    SDL_Init(SDL_INIT_EVERYTHING);
    window_ptr window
    (
        SDL_CreateWindow("Hallo SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
         , SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL)
        , &SDL_DestroyWindow);
    renderer_ptr renderer
    (
        SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED)
        , &SDL_DestroyRenderer
    );
    SDL_SetRenderDrawColor(renderer.get(), 0x0f, 0x0f, 0xf0, 0xff);
    std::vector<texture_ptr> dino;
    dino.emplace_back(load_texture("../assets/dino/Walk (1).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (2).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (3).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (4).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (5).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (6).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (7).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (8).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (9).png", renderer.get()));
    dino.emplace_back(load_texture("../assets/dino/Walk (10).png", renderer.get()));
    texture_ptr background = load_texture("../assets/background.jpg", renderer.get());
    bool running { true };
    int i = 0;
    auto last_ms = SDL_GetTicks();
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event) == 1) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                // default:
            }
        }
        auto now_ms = SDL_GetTicks();
        auto diff = now_ms - last_ms;
        ++i;
        auto frame = static_cast<size_t>(i / 40.0f) % dino.size();
        auto const& tex = dino[frame].get();
        // dst.x = 150 - std::sinf(i / 120.0f) * 100;
        SDL_RenderClear(renderer.get());
        SDL_Rect back_src { 0, 0, 1280, 720 };
        SDL_Rect back_dst { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer.get(), background.get(), &back_src, &back_dst);
        SDL_Rect src {0, 0, 640, 480};
        SDL_Rect dst {static_cast<int>(i/40.0f * 15.0f) % SCREEN_WIDTH, SCREEN_HEIGHT - 480, 640 / 2, 480 / 2};
        SDL_RenderCopy(renderer.get(), tex, &src, &dst);
        SDL_RenderPresent(renderer.get());
        if (diff < 1000/60)
            SDL_Delay(1000/60 - diff);
    }
    
    // SDL_DestroyTexture(tex);
    SDL_Quit();
    return ret;
}