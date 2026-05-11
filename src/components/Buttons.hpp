#pragma once
#include "clay.h"
#include <cstdint>
#include "../ColorUtils.hpp"
#include "../TextUtils.hpp"

namespace Buttons {
	
	using OnClickFunc_t = void(*)(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
	
	struct ButtonArgs {
		uint16_t fontSize = 24;
		bool active = false;
		OnClickFunc_t onHover = {};
		intptr_t callbackArgs = 0;
		Clay_Color bgIdleColor = ColorUtils::TRANSPARENT();
		Clay_Color bgHoverColor = ColorUtils::TRANSPARENT();
		Clay_Color fgIdleColor = ColorUtils::LIGHT_GRAY();
		Clay_Color fgHoverColor = ColorUtils::WHITE_();
		
	};
	
	inline void RawButton(Clay_String buttonText, const ButtonArgs& args) {
		CLAY({ .layout = { .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = Clay_Hovered() || args.active ?  args.bgIdleColor : args.bgHoverColor}) {
	        CLAY_TEXT(buttonText, CLAY_TEXT_CONFIG(TextUtils::Default(args.fontSize, Clay_Hovered() || args.active ? args.fgHoverColor : args.fgIdleColor)));
	        Clay_OnHover(args.onHover, args.callbackArgs);
	    }
		
	}
	
	inline void HeaderButton(Clay_String buttonText, const ButtonArgs& args = {}) {
	    // Red box button with 8px of padding
	    RawButton(buttonText, args);
	}
}
