//#define _CRT_SECURE_NO_WARNINGS

#include "MetaUtils.h"
#include "BMPLib.h"

#include <iostream>
#include <string>
#include <malloc.h>


namespace meta
{

//
//
//
bool BMP_Make(BMP_Image *_img, unsigned int _width, unsigned int _height, unsigned char *_data)
   {
   strcpy(_img->id, "BM");
   _img->file_size      = 1078 + _width*_height; // data_offset + data_size
   _img->reserved1      = 0;
   _img->data_offset      = 1078;
   _img->header_size      = 40;
   _img->width            = _width;
   _img->height         = _height;
   _img->planes         = 1;
   _img->bits_per_pix   = 8;
   _img->compression      = 0;
   _img->data_size      = _width*_height;
   _img->hres            = 0;
   _img->vres            = 0;
   _img->colors         = 256;
   _img->imp_colors      = 256;
   for (int i = 0; i < 256; i++)
      {
      _img->palette[i][0] = i;
      _img->palette[i][1] = i;
      _img->palette[i][2] = i;
      _img->palette[i][3] = 0;
      }
   _img->data = _data;

   return true;
   }

//
//
//
bool BMP_Write(char *_fname, BMP_Image *_img)
   {
   FILE *fout;

   if ((fout = fopen(_fname, "w+b")) == NULL)
      {
          std::cout << "Error: Unable to open file " << _fname << " for writing." << std::endl;
      return false;
      }

   fwrite(_img->id, 2, 1, fout);
   MET_fwritel(fout, _img->file_size, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->reserved1, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->data_offset, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->header_size, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->width, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->height, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwrites(fout, _img->planes, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwrites(fout, _img->bits_per_pix, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->compression, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->data_size, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->hres, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->vres, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->colors, !MET_SYSTEM_BYTE_ORDER_MSB);
   MET_fwritel(fout, _img->imp_colors, !MET_SYSTEM_BYTE_ORDER_MSB);
   for (int i = 0; i < _img->imp_colors; i++)
      fwrite(_img->palette[i], 4, 1, fout);

   int padding = 4 - (_img->width%4);
   if (padding == 4)
      fwrite(_img->data, 1, _img->width*_img->height, fout);
     else // BMP data must be padded to nearest dword as per windows dictum
      {
      unsigned char* temp = _img->data;
      for (int i = 0; i < _img->height; i++)
         {
         fwrite(temp, 1, _img->width, fout);
         for (int j = 0; j < padding; j++)
            fputc(0, fout);
         temp += _img->width;
         }
      }

   fclose(fout);

   return true;
   }

//
//
//
bool BMP_Read(char *_fname, BMP_Image *_img)
   {
   FILE *fin;

   if ((fin = fopen(_fname, "r+b")) == NULL)
      {
          std::cout << "Error: unable to open file " << _fname << " for reading."<< std::endl;
      return false;
      }

   fread(&_img->id, 2, 1, fin);
   _img->file_size = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->reserved1 = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->data_offset = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->header_size = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->width = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->height = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->planes = MET_freads(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->bits_per_pix = MET_freads(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->compression = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->data_size = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->hres = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->vres = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->colors = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   _img->imp_colors = MET_freadl(fin, !MET_SYSTEM_BYTE_ORDER_MSB);
   for (int i = 0; i < _img->imp_colors; i++)
      fread(_img->palette[i], 4, 1, fin);


   /*  THIS IS ORIGINAL

   fseek(fin, _img->data_offset, SEEK_SET);
   _img->data   = (unsigned char *) malloc(_img->width*_img->height);
   int padding = 4 - (_img->width%4);
   if (padding == 4)
      fread(_img->data, 1, _img->width*_img->height, fin);
     else 
      {
      unsigned char *   temp = _img->data;
      unsigned char     junk[5];
      for (int i = 0; i < _img->height; i++) 
         {
         fread(temp, 1, _img->width, fin);
         fread(junk, 1, padding, fin);
         temp += _img->width;
         }
      }


   */


               //assume 8, 16 or 32 bits
   int numbytes = _img->bits_per_pix/8;

    fseek(fin, _img->data_offset, SEEK_SET);
   _img->data   = (unsigned char *) malloc(_img->width*_img->height * numbytes);
   int padding = 4 - ( (_img->width * numbytes)%4);
   if (padding == 4)
      fread(_img->data, 1, _img->width*_img->height * numbytes, fin);
     else 
      {
      unsigned char *   temp = _img->data;
      unsigned char     junk[5];
      for (int i = 0; i < _img->height; i++) 
         {
         fread(temp, 1, _img->width * numbytes, fin);
         fread(junk, 1, padding, fin);
         temp += _img->width * numbytes;
         }
      }

   fclose(fin);

   return true;
   }

void BMP_PrintInfo(BMP_Image *_img)
   {
   fprintf(stderr, "     id           = %s\n", _img->id);
   fprintf(stderr, "     file_size    = %lu\n", _img->file_size);
   fprintf(stderr, "     reserved1    = %lu\n", _img->reserved1);
   fprintf(stderr, "     data_offset  = %lu\n", _img->data_offset);
   fprintf(stderr, "     header_size  = %lu\n", _img->header_size);
   fprintf(stderr, "     width        = %lu\n", _img->width);
   fprintf(stderr, "     height       = %lu\n", _img->height);
   fprintf(stderr, "     planes       = %u\n", _img->planes);
   fprintf(stderr, "     bits_per_pix = %u\n", _img->bits_per_pix);
   fprintf(stderr, "     compression  = %lu\n", _img->compression);
   fprintf(stderr, "     data_size    = %lu\n", _img->data_size);
   fprintf(stderr, "     hres         = %lu\n", _img->hres);
   fprintf(stderr, "     vres         = %lu\n", _img->vres);
   fprintf(stderr, "     colors       = %lu\n", _img->colors);
   fprintf(stderr, "     imp_colors   = %lu\n", _img->imp_colors);
   }
//--------------------------------------------------
      //eb: experimenting with trying to make color bitmap
bool BMP_MakeColor(BMP_Image *img, unsigned int _width, unsigned int _height, unsigned char *_data, unsigned char* pal)
   {
//old code
   strcpy(img->id, "BM");
   img->file_size      = 1078 + _width*_height; // data_offset + data_size
   img->reserved1      = 0;
   img->data_offset   = 1078;
   img->header_size   = 40;
   img->width         = _width;
   img->height         = _height;
   img->planes         = 1;
   img->bits_per_pix   = 8;
   img->compression   = 0;
   img->data_size      = _width*_height;
   img->hres         = 0;
   img->vres         = 0;
   img->colors         = 256;
   img->imp_colors      = 256;
   for (int i = 0; i < 256; i++)
      {
      img->palette[i][0] = pal[i * 4];
      img->palette[i][1] = pal[i * 4 + 1];
      img->palette[i][2] = pal[i * 4 + 2];
      img->palette[i][3] = 0;
      }
   img->data = _data;

   return 1;
   }

}//end of namespace
