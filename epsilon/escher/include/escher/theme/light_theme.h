#pragma once

#include <escher/palette.h>

#include "theme_container.h"

namespace Escher {

constexpr static ThemeContainer k_lightTheme =
    ThemeContainer{.Background = Palette::White,
                   .Graph = Palette::YellowDark,
                   .Text = Palette::Black};

}  // namespace Escher
