#pragma once
#include "rmui/ui_image.h"

UIImage::UIImage() : UIWidget()
{
	interactive = false;
	blocking = false;
	useTexture = true;
}

UIImage::UIImage(int id) : UIWidget(id)
{
	interactive = false;
	blocking = false;
	useTexture = true;
}