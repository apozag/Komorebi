#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "ImageManager.h"

std::map<std::string, Image> ImageManager::loadedImages;

unsigned char* RGBtoRGBA(unsigned char* data, int width, int height) {
	unsigned char* newData = (unsigned char*)malloc(width * height * 4);
	int old_i = 0;
	int new_i = 0;
	for (int i = 0; i < width * height; i++) {
		newData[new_i++] = data[old_i++];
		newData[new_i++] = data[old_i++];
		newData[new_i++] = data[old_i++];
		newData[new_i++] = 0xFF;
	}
	return newData;
}

Image ImageManager::decodeFromMemory(unsigned char* data, int size) {
	Image img = {};
	try {
		img.data = stbi_load_from_memory(data, size, &(img.width), &(img.height), &(img.channels), 0);
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
		{
			unsigned char* rgbData = img.data;
			img.data = RGBtoRGBA(img.data, img.width, img.height);
			free(rgbData);
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			img.channels = 4;
		}
			break;
		case 4:
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
	}
	return img;
}

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