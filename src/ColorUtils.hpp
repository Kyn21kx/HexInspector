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

// Neutrals (foundation)
constexpr Clay_Color PURE_WHITE(float a = 255) { return FromHex(0xffffff, a); }
constexpr Clay_Color OFF_WHITE(float a = 255) { return FromHex(0xf5f5f5, a); }
constexpr Clay_Color PURE_BLACK(float a = 255) { return FromHex(0x000000, a); }
constexpr Clay_Color SOFT_BLACK(float a = 255) { return FromHex(0x1a1a1a, a); }

// Grays (layering)
constexpr Clay_Color SLATE_50(float a = 255) { return FromHex(0xf8fafc, a); }
constexpr Clay_Color SLATE_200(float a = 255) { return FromHex(0xe2e8f0, a); }
constexpr Clay_Color SLATE_400(float a = 255) { return FromHex(0x94a3b8, a); }
constexpr Clay_Color SLATE_600(float a = 255) { return FromHex(0x475569, a); }
constexpr Clay_Color SLATE_800(float a = 255) { return FromHex(0x1e293b, a); }

// Primary actions (confident)
constexpr Clay_Color ACTION_BLUE(float a = 255) { return FromHex(0x3b82f6, a); }
constexpr Clay_Color DEEP_ACTION_BLUE(float a = 255) { return FromHex(0x2563eb, a); }
constexpr Clay_Color SUBTLE_BLUE(float a = 255) { return FromHex(0xdbeafe, a); }

// Success (affirmative)
constexpr Clay_Color SUCCESS_GREEN(float a = 255) { return FromHex(0x22c55e, a); }
constexpr Clay_Color DEEP_SUCCESS_GREEN(float a = 255) { return FromHex(0x16a34a, a); }
constexpr Clay_Color SUBTLE_GREEN(float a = 255) { return FromHex(0xdcfce7, a); }

// Warnings (cautionary)
constexpr Clay_Color WARNING_YELLOW(float a = 255) { return FromHex(0xfbbf24, a); }
constexpr Clay_Color DEEP_WARNING_YELLOW(float a = 255) { return FromHex(0xd97706, a); }
constexpr Clay_Color SUBTLE_YELLOW(float a = 255) { return FromHex(0xfef3c7, a); }

// Errors (urgent)
constexpr Clay_Color ERROR_RED(float a = 255) { return FromHex(0xef4444, a); }
constexpr Clay_Color DEEP_ERROR_RED(float a = 255) { return FromHex(0xdc2626, a); }
constexpr Clay_Color SUBTLE_RED(float a = 255) { return FromHex(0xfee2e2, a); }

// Info (helpful)
constexpr Clay_Color INFO_SKY(float a = 255) { return FromHex(0x0ea5e9, a); }
constexpr Clay_Color SUBTLE_INFO(float a = 255) { return FromHex(0xe0f2fe, a); }

// Accent (attention-grabbing)
constexpr Clay_Color ACCENT_PURPLE(float a = 255) { return FromHex(0xa855f7, a); }
constexpr Clay_Color ACCENT_PINK(float a = 255) { return FromHex(0xec4899, a); }
constexpr Clay_Color ACCENT_ORANGE(float a = 255) { return FromHex(0xf97316, a); }
constexpr Clay_Color ACCENT_TEAL(float a = 255) { return FromHex(0x14b8a6, a); }

// Dimmed / low emphasis
constexpr Clay_Color MUTED_GRAY(float a = 255) { return FromHex(0x9ca3af, a); }
constexpr Clay_Color DUSTY_WHITE(float a = 255) { return FromHex(0xf3f4f6, a); }
constexpr Clay_Color SMOKY_BLACK(float a = 255) { return FromHex(0x111827, a); }

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
