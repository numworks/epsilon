#pragma once

#include <escher/palette.h>

#include "theme_container.h"

namespace Escher {

constexpr static ThemeContainer k_darkTheme =
    ThemeContainer{.Background = Palette::Purple,
                   .Graph = Palette::PurpleDark,
                   .Text = Palette::White};

}  // namespace Escher
