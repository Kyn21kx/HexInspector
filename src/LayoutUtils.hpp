#pragma once
#include "clay.h"
#include <cstdint>

namespace LayoutUtils {
	inline Clay_Sizing SizeAutoGrowXY() {
		return Clay_Sizing { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };
	}
	
	inline Clay_Sizing SizeAutoGrowX(Clay_SizingAxis height) {
		return Clay_Sizing { CLAY_SIZING_GROW(), height };
	}
	
	inline Clay_Sizing SizeFlexHorizontal(float growFactor) {
		return Clay_Sizing { CLAY_SIZING_GROW(growFactor), {} };
	}
	
	inline Clay_Sizing SizeFlexVertical(float growFactor) {
		return Clay_Sizing { {}, CLAY_SIZING_GROW(growFactor) };
	}

	inline Clay_ChildAlignment ChildAlignCenterAll() {
		return { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER };
	}
	
	inline Clay_ChildAlignment ChildAlignCenterX() {
		return { CLAY_ALIGN_X_CENTER, {} };
	}
	
	inline Clay_ChildAlignment ChildAlignCenterY() {
		return { {}, CLAY_ALIGN_Y_CENTER };
	}
	
	inline Clay_ChildAlignment ChildAlignRightX() {
		return { CLAY_ALIGN_X_RIGHT, {} };
	}

	
}

