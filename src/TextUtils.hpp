#pragma once
#include "ColorUtils.hpp"
#include "clay.h"
#include <cstdint>

namespace TextUtils {
	constexpr Clay_TextElementConfig Default(uint16_t fontSize, Clay_Color color = ColorUtils::WHITE_()) {
		return {
			.textColor = color,	
			.fontId = 0,
			.fontSize = fontSize,
		};
	}
}
