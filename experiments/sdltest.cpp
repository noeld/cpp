#include<iostream>
#include<memory>
#include<string>
#include<SDL.h>
#include<cairo/cairo.h>

class unique_window_ptr : public std::unique_ptr<SDL_Window, decltype(&::SDL_DestroyWindow)>
{
public:
    unique_window_ptr(SDL_Window* ptr) : std::unique_ptr<SDL_Window, decltype(&::SDL_DestroyWindow)>(ptr, ::SDL_DestroyWindow)
    {}
    ~unique_window_ptr() = default;
};

int main(int argc, char const *argv[])
{
    try {
        const auto SCREEN_WIDTH  = 640;
        const auto SCREEN_HEIGHT = 400;
        auto res = SDL_Init(SDL_INIT_VIDEO);
        if (res < 0)
            throw std::string("SDL_Init") + SDL_GetError();
        auto del_window = [](SDL_Window* w) { SDL_DestroyWindow(w); std::cerr << "del_window" << std::endl; };
        // std::unique_ptr<SDL_Window, decltype(del_window)> window(
        //     SDL_CreateWindow( "SDL Tutorial"
        //                     , SDL_WINDOWPOS_UNDEFINED
        //                     , SDL_WINDOWPOS_UNDEFINED
        //                     , SCREEN_WIDTH
        //                     , SCREEN_HEIGHT
        //                     , SDL_WINDOW_SHOWN )
        //     , del_window
        // );
        // std::unique_ptr<SDL_Window, decltype(&::SDL_DestroyWindow)> window(
        //     SDL_CreateWindow( "SDL Tutorial"
        //                     , SDL_WINDOWPOS_UNDEFINED
        //                     , SDL_WINDOWPOS_UNDEFINED
        //                     , SCREEN_WIDTH
        //                     , SCREEN_HEIGHT
        //                     , SDL_WINDOW_SHOWN )
        //     , ::SDL_DestroyWindow
        // );
        unique_window_ptr window(
            SDL_CreateWindow( "SDL Tutorial"
                , SDL_WINDOWPOS_UNDEFINED
                , SDL_WINDOWPOS_UNDEFINED
                , SCREEN_WIDTH
                , SCREEN_HEIGHT
                , SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL
            )
        );
        if (!window) {
            throw std::string{"SDL_CreateWindow: "} + SDL_GetError();
        }
        auto surface = SDL_GetWindowSurface(window.get());
        SDL_FillRect( surface, NULL, SDL_MapRGB( surface->format, 0xFF, 0xFF, 0xFF ) );
        SDL_UpdateWindowSurface(window.get());
        // SDL_Delay(2000);
        // event loop
        bool keep_running = true;
        SDL_Event e;
        while(keep_running) {
            while(SDL_PollEvent(&e) != 0) {
                switch(e.type) {
                    case SDL_QUIT:
                        keep_running = false;
                        std::cerr << "Quit event" << std::endl;
                        break;
                }
            }
        }

        SDL_Quit();
    } catch (const std::string& e) {
        std::cerr << "Exception: " << e << std::endl;
    } 
    return 0;
}
