#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "ImageManager.h"

std::map<std::string, Image> ImageManager::loadedImages;

Image ImageManager::loadImage(std::string path) {
	 Image img = {};
	 img.path = path;
	 if (loadedImages.find(path) == loadedImages.end()){
		 try {
			 img.data = stbi_load(path.c_str(), &(img.width), &(img.height), &(img.channels), 0);
		 }
		 catch (const char* err) {
			 std::cout << err << std::endl;
		 }
		 switch (img.channels) {
		 case 1:
			 img.format = DXGI_FORMAT_R8_UNORM;
			 break;
		 case 2:
			 img.format = DXGI_FORMAT_R8G8_UNORM;
			 break;
		 case 3:
			 img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			 break;
		 case 4:
			 img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			 break;
		 }
		 loadedImages.emplace(path, img);
	}
	else {
		img = loadedImages[path]; 
	}

	return img;
}