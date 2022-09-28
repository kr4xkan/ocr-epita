with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "ocr";

  buildInputs = [
    gdb
    gcc
    pkg-config

    # v1
    SDL
    SDL_Pango
    SDL_gfx
    SDL_image
    SDL_mixer
    SDL_net
    SDL_ttf

    # v2
    SDL2
    SDL2_image
    SDL2_mixer
    SDL2_net
    SDL2_ttf
  ];
}
