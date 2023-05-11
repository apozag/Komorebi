#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "3rd/stb_image/stb_image.h"
#endif

#include "Core/Util/ImageManager.h"

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

Image* ImageManager::tryGetLoadedImage(std::string filename) {
  if (loadedImages.find(filename) != loadedImages.end()) {
    return &loadedImages[filename];
  }
  return nullptr;
}

Image ImageManager::decodeFromMemory(std::string path, unsigned char* data, int size) {

  Image* loadedImg = tryGetLoadedImage(path);
  if (loadedImg) {
    return *loadedImg;
  }

  Image img;
  img.path = path;
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

  loadedImages.emplace(path, img);

  return img;
}

Image ImageManager::loadImage(std::string path) {

  Image* loadedImg = tryGetLoadedImage(path);
  if (loadedImg) {
    return *loadedImg;
  }

  Image img;
  img.path = path;
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

  loadedImages.emplace(path, img);

  return img;
}