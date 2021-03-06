//an example using the PNG loading function:

#include <iostream>
#include <fstream>
#include <vector>

void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector)
{
  std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

  //get filesize
  std::streamsize size = 0;
  if(file.seekg(0, std::ios::end).good()) size = file.tellg();
  if(file.seekg(0, std::ios::beg).good()) size -= file.tellg();

  //read contents of the file into the vector
  if(size > 0)
  {
    buffer.resize((size_t)size);
    file.read((char*)(&buffer[0]), size);
  }
  else buffer.clear();
}

int main(int argc, char *argv[])
{
  const char* filename = argc > 1 ? argv[1] : "test.png";

  //load and decode
  std::vector<unsigned char> buffer, image;
  loadFile(buffer, filename);
  unsigned long w, h;
  int error = decodePNG(image, w, h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());

  //if there's an error, display it
  if(error != 0) std::cout << "error: " << error << std::endl;

  //the pixels are now in the vector "image", use it as texture, draw it, ...

  if(image.size() > 4) std::cout << "width: " << w << " height: " << h << " first pixel: " << std::hex << int(image[0]) << int(image[1]) << int(image[2]) << int(image[3]) << std::endl;
}

/*
  //this is test code, it displays the pixels of a 1 bit PNG. To use it, set the flag convert_to_rgba32 to false and load a 1-bit PNG image with a small size (so that its ASCII representation can fit in a console window)
  for(int y = 0; y < h; y++)
  {
    for(int x = 0; x < w; x++)
    {
      int i = y * h + x;
      std::cout << (((image[i/8] >> (7-i%8)) & 1) ? '.' : '#');
    }
    std::cout << std::endl;
  }
*/


