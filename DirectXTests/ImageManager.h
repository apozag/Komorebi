#pragma once

#include "dxgiformat.h" 
#include <iostream>
#include <map>
#include <string>

struct Image {
	unsigned char* data;
	std::string path;
	DXGI_FORMAT format;
	int channels;
	int width, height;
	unsigned int getMemPitch(){ return channels * width; }
};

class ImageManager {
public:
	static Image loadImage(std::string path);
private:
	static std::map<std::string, Image> loadedImages;
};