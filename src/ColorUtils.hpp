#pragma once

#include "raylib.h"
#include "clay.h"

namespace ColorUtils {

constexpr Clay_Color FromHex(unsigned int h, float a = 255) {
    return Clay_Color{
        .r = (float)((h >> 16) & 0xFF),
        .g = (float)((h >> 8) & 0xFF),
        .b = (float)(h & 0xFF),
        .a = a,
    };
}

// Surface / Background
constexpr Clay_Color DARK_BG(float a = 255) { return FromHex(0x18181c, a); }
constexpr Clay_Color PANEL_BG(float a = 255) { return FromHex(0x2a2a30, a); }
constexpr Clay_Color HOVER_BG(float a = 255) { return FromHex(0x3a3a42, a); }
constexpr Clay_Color ACTIVE_BG(float a = 255) { return FromHex(0x4a4a52, a); }

// Borders / Lines
constexpr Clay_Color BORDER(float a = 255) { return FromHex(0x3a3a42, a); }
constexpr Clay_Color BORDER_LIGHT(float a = 255) { return FromHex(0x4a4a52, a); }
constexpr Clay_Color DIVIDER(float a = 255) { return FromHex(0x333338, a); }

// Text
constexpr Clay_Color TEXT_PRIMARY(float a = 255) { return FromHex(0xf4f4f5, a); }
constexpr Clay_Color TEXT_SECONDARY(float a = 255) { return FromHex(0x99999e, a); }
constexpr Clay_Color TEXT_MUTED(float a = 255) { return FromHex(0x6b6b72, a); }
constexpr Clay_Color TEXT_ACCENT(float a = 255) { return FromHex(0x7dd3fc, a); }

// Accent / Interactive
constexpr Clay_Color ACCENT(float a = 255) { return FromHex(0x7dd3fc, a); }
constexpr Clay_Color ACCENT_DIM(float a = 255) { return FromHex(0x5bb8e0, a); }
constexpr Clay_Color ACCENT_MUTED(float a = 255) { return FromHex(0x3a7a9e, a); }
constexpr Clay_Color LINK(float a = 255) { return FromHex(0x60a5fa, a); }

// Status
constexpr Clay_Color SUCCESS(float a = 255) { return FromHex(0x4ade80, a); }
constexpr Clay_Color WARNING(float a = 255) { return FromHex(0xfbbf24, a); }
constexpr Clay_Color ERROR(float a = 255) { return FromHex(0xf87171, a); }
constexpr Clay_Color INFO(float a = 255) { return FromHex(0x60a5fa, a); }

// Misc
constexpr Clay_Color WHITE_(float a = 255) { return FromHex(0xffffff, a); }
constexpr Clay_Color BLACK_(float a = 255) { return FromHex(0x000000, a); }
constexpr Clay_Color TRANSPARENT(float a = 255) {
    return Clay_Color{ .r = 0, .g = 0, .b = 0, .a = a };
}

constexpr Clay_Color LIGHT_GRAY(float opacity = 255.0f) {
    return { 194, 194, 194, opacity };
}

// Conversions — Clay_Color and raylib Color both use 0-255 convention
constexpr Color FromClayToRaylib(Clay_Color c) {
    return Color{
        .r = (unsigned char)(c.r),
        .g = (unsigned char)(c.g),
        .b = (unsigned char)(c.b),
        .a = (unsigned char)(c.a),
    };
}

constexpr Clay_Color ToClay(Color c) {
    return Clay_Color{
        .r = (float)c.r,
        .g = (float)c.g,
        .b = (float)c.b,
        .a = (float)c.a,
    };
}

}
