#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "3rd/stb_image/stb_image.h"
#endif

#include "Core/Util/ImageManager.h"

std::map<std::string, Image> ImageManager::loadedImages;

template<typename T>
void RGB2RGBA(T** ppData, int width, int height) {
  T* oldData = *ppData;
  T* newData = (T*)malloc(width * height * sizeof(T) * 4);
  int old_i = 0;
  int new_i = 0;
  for (int i = 0; i < width * height; i++) {
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = 1;
  }
  free(oldData);
  *ppData = newData;
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
    RGB2RGBA((unsigned char**)(&img.data), img.width, img.height);
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
  bool isHDR = false;
  try {    
    size_t extIdx = path.find_last_of('.');
    if (extIdx != std::string::npos && extIdx < path.size()-1) {
      std::string extension = path.substr(extIdx+1);
      isHDR = extension == "exr" || extension == "hdr";
    }
    if (isHDR) {      
      img.data = stbi_loadf(path.c_str(), &(img.width), &(img.height), &(img.channels), 0);
    }
    else {
      img.data = stbi_load(path.c_str(), &(img.width), &(img.height), &(img.channels), 0);
    }
  }
  catch (const char* err) {
    std::cout << err << std::endl;
  }
  if (isHDR) {
    switch (img.channels) {
    case 1:
      img.format = DXGI_FORMAT_R32_FLOAT;
      break;
    case 2:
      img.format = DXGI_FORMAT_R32G32_FLOAT;
      break;
    case 3:
      //RGB2RGBA((float**)(&img.data), img.width, img.height);
      //img.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      img.format = DXGI_FORMAT_R32G32B32_FLOAT;
    break;
    case 4:
      img.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      break;
    }
  }
  else {
    switch (img.channels) {
    case 1:
      img.format = DXGI_FORMAT_R8_UNORM;
      break;
    case 2:
      img.format = DXGI_FORMAT_R8G8_UNORM;
      break;
    case 3:
      RGB2RGBA((unsigned char**)(&img.data), img.width, img.height);
      img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
      img.channels = 4;
    break;
    case 4:
      img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    }
  }

  loadedImages.emplace(path, img);

  return img;
}