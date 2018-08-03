#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <sstream>
#include <opencv2/opencv.hpp>
#define PATH_LENGTH 100
#define OUT_PATH "/Users/zhujingqiao/Downloads/"
#define IN_PATH "/Users/zhujingqiao/Downloads/"

using namespace std;
using namespace cv;
extern "C"{
  #include "freeimage.h"
  #include "lib/gif.h"
  #include <libavutil/opt.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/channel_layout.h>
  #include <libavutil/common.h>
  #include <libavutil/imgutils.h>
  #include <libavutil/mathematics.h>
  #include <libavutil/samplefmt.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}

/**
* 支持:bmp(1-bit、8-bit和24-bit)、
* gif(8-bit)、hdf、jpg(或jpeg)(8-bit、12-bit和16-bit)、
* jp2或jpx、pbm、pcx(8-bit)、
* pgm、png、pnm、ppm、ras、
* tif(或tiff)、xwd等
**/
int imgCompress(string &infile, string &outfile, vector<int> &compression_params);
vector<int> params_handler(char *argv[]);
Mat Gif_To_Mat(FIBITMAP *fiBmp, const FREE_IMAGE_FORMAT fif, int width, int height);
bool Gif_Loader(const string &filename,vector<int> &compression_params);
void JPG_To_GIF(string outputFile,vector<Mat>& inputPics,unsigned int gNum,unsigned int gWidth,unsigned int gHeight,unsigned int gDelay);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "参数未设置完全." << endl;
    return 0;
  }
  vector<int> compression_params = params_handler(argv);
  string filename = "test2.png";
  string infile = IN_PATH;
  string outfile = OUT_PATH;
  infile.append(filename);
  outfile.append("compressed.");
  outfile.append(filename);
  int ans;
  if((strncmp(argv[1], "JPG", 3) == 0) || (strncmp(argv[1], "PNG", 3) == 0)){
    ans = imgCompress(infile, outfile, compression_params);
  }
  else{
    ans = Gif_Loader("/Users/zhujingqiao/Downloads/testgif.gif",compression_params);
  }
  switch (ans)
  {
  case 0:
    cout << "压缩完成." << endl;
    break;
  default:
    cout << "压缩失败." << endl;
  }
  return 0;
}
vector<int> params_handler(char *argv[])
{
  vector<int> compression_params;
  char *format = argv[1];
  int quality;
  stringstream ss;
  ss << argv[2];
  ss >> quality;
  cout << quality << endl;
  cout << format << endl;
  if (strncmp(format, "JPG", 3) == 0)
  {
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    quality *= 10;
    compression_params.push_back(quality);
  }
  else if (strncmp(format, "PNG", 3) == 0)
  {
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    quality = 10 - quality;
    compression_params.push_back(quality);
    compression_params.push_back(IMWRITE_PNG_STRATEGY);
    compression_params.push_back(IMWRITE_PNG_STRATEGY_RLE);
  }
  else if(strncmp(format,"GIF",3)==0 )
  {
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    quality *= 10;
    compression_params.push_back(quality);
  }
  else{
    cout<<"格式未支持."<<endl;
  }
  return compression_params;
}
int imgCompress(string &inpath, string &outpath, vector<int> &compression_params)
{
  Mat src, dst, temp;
  //cout << inpath << endl;
  src = imread(inpath);
  if (src.empty())
  {
    cout << ("Can not load image !\n");
    return 1;
  }
  try
  {
    imwrite(outpath, src, compression_params);
    dst = imread(outpath);
    //bilateralFilter(dst,temp,5,50,50);
    namedWindow("output", CV_WINDOW_AUTOSIZE);
    // namedWindow("blur", CV_WINDOW_AUTOSIZE);
    // imshow("blur", temp);
    imshow("output", dst);
    waitKey(8000);
    destroyAllWindows();
  }
  catch (runtime_error &ex)
  {
    cout << "转换错误 " << ex.what();
    return 1;
  }
  return 0;
}

bool Gif_Loader(const string &filename,vector<int> &compression_params)
{
  FIBITMAP *dib = 0;
  FIMULTIBITMAP *bitmap = 0;
  FIBITMAP *pFrame;

  FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.c_str(), 0);
  cout<<"fif"<<fif<<endl;
  if (fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(filename.c_str());
  if (fif == FIF_UNKNOWN)
    return false;
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, filename.c_str());
  if (!dib)
    return false; //dib Load failed

  //bpp = FreeImage_GetBPP(dib);
  auto bits = (BYTE *)FreeImage_GetBits(dib);
  unsigned int width = FreeImage_GetWidth(dib);
  unsigned int height = FreeImage_GetHeight(dib);
  
  cout << "Load The File:   " << filename.c_str() << endl;
  cout << "The File's width: " << width << endl;
  cout << "The File's height: " << height << endl;

  if ((bits == 0) || (width == 0) || (height == 0))
    return false;

  bitmap = FreeImage_OpenMultiBitmap(fif, filename.c_str(), 0, 0, 1, GIF_DEFAULT);
  if (bitmap == NULL)
  {
    cout << "BitMap == Null" << endl;
    return FALSE;
  }

  int count = FreeImage_GetPageCount(bitmap); //获取帧数；
  vector<Mat> frames_pixel; //所有帧像素
  for (int i = 0; i <= count; i++)
  {
    pFrame = FreeImage_LockPage(bitmap, i);
    //cout << "pFrame:" << pFrame << endl;
    Mat Src_Gif = Gif_To_Mat(pFrame, fif, width, height); //转换为Mat；
    string gif_output = IN_PATH;
    string Src_Gif_Name = gif_output+"gif/";
    Src_Gif_Name.append(to_string(i));
    imwrite(Src_Gif_Name + ".jpg", Src_Gif,compression_params); //写入临时帧
    Src_Gif_Name.append(".jpg");
    Mat temp = imread(Src_Gif_Name);
    frames_pixel.push_back(temp);
    // FREE_IMAGE_FORMAT frf = FreeImage_GetFIFFromFilename(Src_Gif_Name.c_str());
    // FIBITMAP *frame = FreeImage_Load(frf, Src_Gif_Name.c_str());
    // auto bits = (BYTE *)FreeImage_GetBits(frame);
    //cout<<bits<<endl;
    //frames_pixel.push_back(bits);
    FreeImage_UnlockPage(bitmap, pFrame, 1);
    //FreeImage_Unload(frame);
  }
  cout<<frames_pixel.size()<<endl;
  string out_gif = IN_PATH;
  out_gif.append("anim.gif");
  JPG_To_GIF(out_gif,frames_pixel,count,width,height,10);
  FreeImage_Unload(dib);
  FreeImage_DeInitialise();
  bool Load_flag = TRUE;
  return Load_flag;
}
Mat Gif_To_Mat(FIBITMAP *fiBmp, const FREE_IMAGE_FORMAT fif, int width, int height)
{
  if (fiBmp == NULL || fif != FIF_GIF)
  {
    return Mat();
  }

  BYTE intensity;
  BYTE *PIintensity = &intensity;
  if (FreeImage_GetBPP(fiBmp) != 8)
    fiBmp = FreeImage_ConvertTo8Bits(fiBmp);

  RGBQUAD *pixels = new RGBQUAD;
  pixels = FreeImage_GetPalette(fiBmp);

  Mat img = Mat::zeros(height, width, CV_8UC3);

  uchar *p;

  for (int i = 0; i < height; i++)
  {
    p = img.ptr<uchar>(i);
    for (int j = 0; j < width; j++)
    {
      FreeImage_GetPixelIndex(fiBmp, j, height - i, PIintensity);
      p[3 * j] = pixels[intensity].rgbBlue;
      p[3 * j + 1] = pixels[intensity].rgbGreen;
      p[3 * j + 2] = pixels[intensity].rgbRed;
    }
  }
  return img;
}
// bool Jpg_To_Video()
// {
//   VideoWriter video("output.avi", CV_FOURCC('M', 'P', '4', '2'), 25.0, Size(150, 131));
//   String File_Name = "*.jpg";
//   vector<String> fn;
//   glob(File_Name, fn, false); //遍历文件夹的图片/文件
//   size_t size = fn.size();
//   cout << "Jpg_To_Video size:" << size << endl;
//   cout << "开始将图片文件写入视频" << endl;
//   for (size_t i = 0; i < size; i++)
//   {
//     Mat image = imread(fn[i]);
//     //imshow(to_string(i), image);
//     //resize(image, image, Size(640, 480));  //这里 必须将image的大小 转换为 VideoWriter video(...)一样的大小。
//     video.write(image);
//   }

//   cout << "写入 成功!" << endl;

//   return TRUE;
// }
void JPG_To_GIF(string outputFile,vector<Mat> &inputPics,unsigned int gNum,unsigned int gWidth,unsigned int gHeight,unsigned int gDelay){
  GifWriter gw;
  GifBegin(&gw,outputFile.c_str(), gWidth, gHeight, gDelay);
  for (int n = 0; n < 20; ++n)
  {
      // 写入gw的图片数据为rgba格式
      int length = 3 * gWidth*gHeight;
      uint8_t *imgFrame = new uint8_t[length];    
      memset(imgFrame,0,length);
      for (int k = 0; k < gWidth*gHeight; k++)
      {
          Mat temp = inputPics[n];
          //cout<<tem<<endl;
          *(imgFrame + k*4 + 0) = temp.at<uint8_t>(k*3 + 2);
          *(imgFrame + k*4 + 1) = temp.at<uint8_t>(k*3 + 1);
          *(imgFrame + k*4 + 2) = temp.at<uint8_t>(k*3 + 0);
          //*(imgFrame + k * 4 + 3) = 0xff;
          // rgba中的a忽略
      }
      GifWriteFrame(&gw, imgFrame, gWidth, gHeight, gDelay);
      delete [] imgFrame;
  }
  GifEnd(&gw);
}