#ifndef BMPLIB_H
#define BMPLIB_H



namespace meta
{

typedef struct {
   char id[3];
   long file_size;
   long reserved1;
   long data_offset;
   long header_size;
   long width;
   long height;
   unsigned short planes;
   unsigned short bits_per_pix;
   long compression;
   long data_size;
   long hres;
   long vres;
   long colors;
   long imp_colors;
   unsigned char palette[256][4];
   unsigned char *data;
} BMP_Image;

extern bool BMP_Make(BMP_Image *_img, unsigned int _width, unsigned int _height, unsigned char *_data);

extern bool BMP_Write(char *_fname, BMP_Image *_img);

extern bool BMP_Read(char *_fname, BMP_Image *_img);

extern void BMP_PrintInfo(BMP_Image *_img);

         //eb--experimenting
extern bool BMP_MakeColor(BMP_Image *_img, unsigned int _width, unsigned int _height, unsigned char *_data, unsigned char* pal);

}//end of namespace

#endif
