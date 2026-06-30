#pragma once
#include "rmui/style/ui_style.h"
#include "utility/utils.h"

//using json = nlohmann::json;
//
//UIStyleService::~UIStyleService()
//{
//    
//}
//
//void UIStyleService::load(json data)
//{
//    for (auto& [key, val] : data.items())
//    {
//        StyleComponent style;
//        style.name = key;
//        std::string hexBGColor = val["backgroundColor"].get<std::string>();
//        uint8_t alpha = val["alpha"].get<uint8_t>();
//
//        SDL_Color bgColor = HexToRGBA(hexBGColor, alpha);
//
//        style.backgroundColor = bgColor;
//
//        styles[style.name] = style;
//	}
//}