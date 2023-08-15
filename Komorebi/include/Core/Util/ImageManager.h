#pragma once

#include "dxgiformat.h" 
#include <iostream>
#include <map>
#include <string>

struct Image {
	void* data;
	std::string path;
	DXGI_FORMAT format;
	int channels;
	int width, height;
	unsigned int getMemPitch() const { return channels * width; }
};

class ImageManager {
public:
	static Image* tryGetLoadedImage(std::string filename);
	static Image loadImage(std::string path);
	static Image decodeFromMemory(std::string path, unsigned char* data, int size);
private:
	static std::map<std::string, Image> loadedImages;
};