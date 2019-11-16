// main.cpp
#include "SDL.h"
#include "SDL_image.h"
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include "vector2d.h"

using window_ptr   = std::unique_ptr<SDL_Window   , decltype(&SDL_DestroyWindow)>;
using renderer_ptr = std::unique_ptr<SDL_Renderer , decltype(&SDL_DestroyRenderer)>;
using texture_ptr  = std::unique_ptr<SDL_Texture  , decltype(&SDL_DestroyTexture)>;
using surface_ptr  = std::unique_ptr<SDL_Surface  , decltype(&SDL_FreeSurface)>;
using v2d          = vector2d<float>;

template<typename T, size_t N = 10>
struct window_avg {
    static const size_t SIZE = N;
    T e_[SIZE] = { 0 };
    T sum_ = { 0 };
    size_t pos_ { 0 };
    void insert(T i) {
        sum_ +=  i - e_[pos_];
        e_[pos_] = i;
        ++pos_;
        if (pos_ >= SIZE)
            pos_ = 0;
    }
    T avg() const {
        // T tmp = 0;
        // for(auto e : e_)
        //     tmp += e;
        // return tmp / SIZE;
        return sum_ / SIZE;
    }
};

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

struct frame_source {
    SDL_Texture* texture_;
    SDL_Rect     src_rect_;
};

struct animation_cycle {
    explicit animation_cycle( unsigned cycle_duration_ms
                            , std::vector<std::string>&& filenames
                            , v2d speed
                            , SDL_Renderer* renderer)
    : cycle_duration_ms_{cycle_duration_ms}
    , filenames_{std::forward<std::vector<std::string>>(filenames)}
    , texture_intervall_ms_{cycle_duration_ms / static_cast<unsigned>(filenames_.size())}
    , speed_ { speed }
    {
        for(auto const& f: filenames_) {
            textures_.emplace_back(load_texture(f.c_str(), renderer));
        }
    }
    frame_source get_frame(unsigned milliseconds) {
        auto frame = (milliseconds / texture_intervall_ms_) % filenames_.size();
        return {
            textures_[frame].get(),
            { 0, 0, 640, 480 }
        };
    } 
    std::vector<std::string> filenames_;
    std::vector<texture_ptr> textures_;
    unsigned cycle_duration_ms_;
    unsigned texture_intervall_ms_;
    v2d speed_;
};

struct animation_intervall {
    animation_cycle* cycle;
    unsigned start_ms_;
    unsigned end_ms_;
    v2d speed_;
};

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
        SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
        , &SDL_DestroyRenderer
    );
    SDL_SetRenderDrawColor(renderer.get(), 0xff, 0xff, 0xff, 0xa0);
    animation_cycle dino_walk_cycle(600, {
                            "../assets/dino/Walk (1).png"
                          , "../assets/dino/Walk (2).png"
                          , "../assets/dino/Walk (3).png"
                          , "../assets/dino/Walk (4).png"
                          , "../assets/dino/Walk (5).png"
                          , "../assets/dino/Walk (6).png"
                          , "../assets/dino/Walk (7).png"
                          , "../assets/dino/Walk (8).png"
                          , "../assets/dino/Walk (9).png"
                          , "../assets/dino/Walk (10).png"}
                          , { 100.0f, 0.0f }
                          , renderer.get());
    animation_cycle dino_idle_cycle(2400, {
                            "../assets/dino/Idle (1).png"
                          , "../assets/dino/Idle (2).png"
                          , "../assets/dino/Idle (3).png"
                          , "../assets/dino/Idle (4).png"
                          , "../assets/dino/Idle (5).png"
                          , "../assets/dino/Idle (6).png"
                          , "../assets/dino/Idle (7).png"
                          , "../assets/dino/Idle (8).png"
                          , "../assets/dino/Idle (9).png"
                          , "../assets/dino/Idle (10).png"}
                          , { 0.0f, 0.0f }
                          , renderer.get());
    animation_cycle dino_run_cycle(600, {
                            "../assets/dino/Run (1).png"
                          , "../assets/dino/Run (2).png"
                          , "../assets/dino/Run (3).png"
                          , "../assets/dino/Run (4).png"
                          , "../assets/dino/Run (5).png"
                          , "../assets/dino/Run (6).png"
                          , "../assets/dino/Run (7).png"
                          , "../assets/dino/Run (8).png"}
                          , { 300.0f, 0.0f }
                          , renderer.get());
    animation_cycle dino_jump_cycle(1200, {
                            "../assets/dino/Jump (1).png"
                          , "../assets/dino/Jump (2).png"
                          , "../assets/dino/Jump (3).png"
                          , "../assets/dino/Jump (4).png"
                          , "../assets/dino/Jump (5).png"
                          , "../assets/dino/Jump (6).png"
                          , "../assets/dino/Jump (7).png"
                          , "../assets/dino/Jump (8).png"
                          , "../assets/dino/Jump (9).png"
                          , "../assets/dino/Jump (10).png"
                          , "../assets/dino/Jump (11).png"
                          , "../assets/dino/Jump (12).png"}
                          , { 300.0f, 0.0f }
                          , renderer.get());

    auto new_intervall = [&](unsigned start_ms, animation_cycle* ac) -> animation_intervall {
        return { ac, start_ms, start_ms + ac->cycle_duration_ms_, ac->speed_};
    };

    texture_ptr background = load_texture("../assets/background.jpg", renderer.get());
    v2d dino_pos { 50, SCREEN_HEIGHT - 480 };
    v2d dino_speed;
    bool running { true };

    int i = 0;
    auto last_ms = SDL_GetTicks();
    auto last_clk = std::chrono::high_resolution_clock::now();
    animation_intervall current_intervall = new_intervall(last_ms, &dino_idle_cycle);
    animation_cycle* next_cycle = current_intervall.cycle;
    
    unsigned cycle_start_ms = last_ms;
    // unsigned cycle_end_ms   = cycle_start_ms + 

    window_avg<int, 100> wa;
    window_avg<float_t, 100> waf;
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
        
        // advance time and frame counters
        auto now_ms = SDL_GetTicks();
        auto diff = now_ms - last_ms;
        wa.insert(diff);
        last_ms = now_ms;
        auto now_clk = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> diff_clk = now_clk - last_clk;
        waf.insert(diff_clk.count());
        last_clk = now_clk;
        ++i;

        auto key_state = SDL_GetKeyboardState(nullptr);
        bool walking = key_state[SDL_SCANCODE_RIGHT] == 1;
        bool running = key_state[SDL_SCANCODE_LSHIFT] + key_state[SDL_SCANCODE_RSHIFT] >= 1;
        bool jumping = key_state[SDL_SCANCODE_SPACE] == 1;
        
        if (jumping) {
            next_cycle = &dino_jump_cycle;
        }
        else if ( walking && running) {
            next_cycle = &dino_run_cycle;
        }
        else if (walking) {
            next_cycle = &dino_walk_cycle;
        } else {
            next_cycle = &dino_idle_cycle;
        }
        if (   current_intervall.cycle == & dino_idle_cycle 
             || now_ms > current_intervall.end_ms_) {
            // next animation intervall
            current_intervall = new_intervall(now_ms, next_cycle);
        }
        auto frame = current_intervall.cycle->get_frame(now_ms);
        dino_pos += current_intervall.speed_ * diff_clk.count();

        SDL_RenderClear(renderer.get());
        // SDL_Rect back_src { 0, 0, 1280, 720 };
        int back_x = static_cast<int>(dino_pos.getX()) % SCREEN_WIDTH;
        // SDL_Rect back_dst { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_Rect back_src { back_x, 0, 1280 - back_x, 720 };
        SDL_Rect back_dst { 0, 0, SCREEN_WIDTH - back_x, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer.get(), background.get(), &back_src, &back_dst);
        SDL_Rect back_src2 { 0, 0, back_x, 720 };
        SDL_Rect back_dst2 { SCREEN_WIDTH - back_x, 0, back_x, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer.get(), background.get(), &back_src2, &back_dst2);
        // SDL_Rect dst {static_cast<int>(dino_pos.getX()) % SCREEN_WIDTH, SCREEN_HEIGHT - 480, 640 / 2, 480 / 2};
        SDL_Rect dst {100, SCREEN_HEIGHT - 480, 640 / 2, 480 / 2};
        SDL_RenderCopy(renderer.get(), frame.texture_, &frame.src_rect_, &dst);
        SDL_Rect br { 50 - 5, 700 - 5, 10 + static_cast<int>(40.0f * waf.avg() * 1000.0f), 50 + 10};
        SDL_BlendMode bm;
        SDL_GetRenderDrawBlendMode(renderer.get(), &bm);
        SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer.get(), 0xa0, 0xa0, 0xff, 0xa0);
        SDL_RenderFillRect(renderer.get(), &br);
        SDL_SetRenderDrawColor(renderer.get(), 0xff, 0xff, 0xff, 0xa0);
        for(int nn = 0, dd = wa.avg(); nn < dd; ++nn) {
            SDL_Rect rect { 50 + 40 * static_cast<int>(nn), 700, 30 , 50 };
            SDL_RenderFillRect(renderer.get(), &rect);
        }
        SDL_SetRenderDrawBlendMode(renderer.get(), bm);
        SDL_RenderPresent(renderer.get());
        if (diff < 1000/60)
            SDL_Delay(1000/60 - diff);
    }
    
    // SDL_DestroyTexture(tex);
    SDL_Quit();
    return ret;
}