// -*-c++-*-
#ifndef GJL_IMAGE_H
#define GJL_IMAGE_H

#include <gjl/lodepng.h>

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

/*
 * class gjl::DataImage -- wrapper class around lodepng.cc
 * to manage drawing on, encoding, and writing png files.
 * The image may be set from values in an input array.
 * The array may be interpreted as hues which are encoded with max saturation
 * and value.
 * 
 * class gjl::FatDataImage -- subclass of gjl::DataImage
 * This represents each site in the input array as a square of pixels.
 * Member functions draw_thin_grey_path() etc. draw a single pixel wide
 * path throught the image so that the color of the site is visible.
 *
 * This class was written to support making images of percolation clusters
 * and drawing sample paths of random walks on the percolation sample.
 *
 */


namespace gjl {
  namespace image {

    typedef unsigned char image_t;  // channels in pixels in image
    typedef unsigned dim_t;  // dimensions of image
    typedef unsigned lodepng_error_t;

    const int number_of_channels = 4;
    const image_t max_channel_val = 255;

    /* Found on internet. I should try to credit someone. */
    /* Could move this into compiled library, but maybe some optimization from input
       known at compile time ?
    */
    inline void hsvtorgb(image_t *r, image_t *g, image_t *b, image_t h, 
                         image_t s, image_t v)
    {
      image_t region, fpart, p, q, t;
    
      if(s == 0) {
        /* color is grayscale */
        *r = *g = *b = v;
        return;
      }
      
      /* make hue 0-5 */
      region = h / 43;
      /* find remainder part, make it from 0-255 */
      fpart = (h - (region * 43)) * 6;
      
      /* calculate temp vars, doing integer multiplication */
      p = (v * (max_channel_val - s)) >> 8;
      q = (v * (max_channel_val - ((s * fpart) >> 8))) >> 8;
      t = (v * (max_channel_val - ((s * (max_channel_val - fpart)) >> 8))) >> 8;
      
      /* assign temp vars based on color cone region */
      switch(region) {
      case 0:
        *r = v; *g = t; *b = p; break;
      case 1:
        *r = q; *g = v; *b = p; break;
      case 2:
        *r = p; *g = v; *b = t; break;
      case 3:
        *r = p; *g = q; *b = v; break;
      case 4:
        *r = t; *g = p; *b = v; break;
      default:
        *r = v; *g = p; *b = q; break;
      }
      return;
    }

    class HSVvalues {
    public:
      inline image_t get_h() { return h_;}
      inline image_t get_s() { return s_;}
      inline image_t get_v() { return v_;}
      inline void set_h(image_t h) { h_ = h;}
      inline void set_s(image_t s) { s_ = s;}
      inline void set_v(image_t v) { v_ = v;}
      inline void set_hsv(image_t h, image_t s, image_t v) {set_h(h);set_s(s);set_v(v);}

    private:
      image_t h_ = 0;
      image_t s_ = 0;
      image_t v_ = 0;
    };

    inline image_t scaled_hue(double data_value, double max_data_value) {
      return max_channel_val * data_value / max_data_value;
    }

    class RGBvalues {
    public:
      inline image_t get_r() { return r_;}
      inline image_t get_g() { return g_;}
      inline image_t get_b() { return b_;}
      inline image_t get_alpha() { return alpha_;}
      inline void set_r(image_t r) { r_ = r;}
      inline void set_g(image_t g) { g_ = g;}
      inline void set_b(image_t b) { b_ = b;}
      inline void set_alpha(image_t alpha) { alpha_ = alpha;}
      inline void set_from_hsv(image_t h, image_t s, image_t v) {hsvtorgb(&r_,&g_,&b_,h,s,v);}
      inline void set_from_hsv(HSVvalues& hsv) {
        hsvtorgb(&r_,&g_,&b_,hsv.get_h(),hsv.get_s(),hsv.get_v());
      }
    private:
      image_t r_ = 0;
      image_t g_ = 0;
      image_t b_ = 0;
      image_t alpha_ = max_channel_val;

    };

    class ImageText  {
    public:
      inline void set_author(const std::string& author) { author_ = author;}
      inline std::string& get_author() { return author_;}

      inline void set_creator(const std::string& s) {creator_ = s;}

      inline void add_text(const char * k, std::string& v) {
        lodepng_add_text(info_,k,v.c_str());
      }
      
      inline void add_text_to_state(LodePNGInfo& info) {
        info_ = &info;
        if ( author_.length() > 0)
          add_text("Author", author_);
        if ( creator_.length() > 0)
          add_text("Creator", creator_);
      }
      
    private:
      std::string author_ = "John Lapeyre";
      std::string creator_ = "";
      LodePNGInfo* info_;
    };

    /* Some of these things, I did not end up using. Don't know how useful they are. */
    class DataImage {
    public:
      DataImage() { }
      DataImage(std::string& filename) { filename_ = filename;}

      inline void set_filename(std::string& filename) { filename_ = filename;}
      inline std::string& get_filename() { return filename_;}
      inline void set_max_trans_val(double val) { max_trans_val_ = val;}

      inline virtual void set_dims(dim_t h, dim_t w) {
        height_ = h;
        width_ = w;
        image_.clear();
        image_.resize(number_of_channels*h*w);
      }
      inline virtual dim_t get_width() { return width_;}
      inline virtual dim_t get_height() { return height_;}

      inline void clear() { 
        image_.clear();
        image_.shrink_to_fit();
        height_ = width_ = 0;
      }

      inline virtual void set_channel(dim_t x, dim_t y, int ch, image_t val) {
        image_[number_of_channels * width_ * y + number_of_channels * x + ch] = val;
      }

      inline void set_pixel_rgb(dim_t x, dim_t y, image_t r, image_t g, image_t b,
                                image_t alpha = max_channel_val) {
        set_channel(x,y,0,r);
        set_channel(x,y,1,g);
        set_channel(x,y,2,b);
        set_channel(x,y,3,alpha);        
      }

      inline void set_pixel(dim_t x, dim_t y, RGBvalues& rgb) {
        set_pixel_rgb(x,y,rgb.get_r(),rgb.get_g(),rgb.get_b(),rgb.get_alpha());
      }

      inline void set_pixel_grey(dim_t x, dim_t y, image_t grey) {
        set_pixel_rgb(x,y,grey,grey,grey);
      }

      inline void set_pixel_hsv(dim_t x, dim_t y, image_t h, image_t s, image_t v) {
        RGBvalues rgb;
        rgb.set_from_hsv(h,s,v);
        set_pixel(x,y,rgb);
      }

      inline void set_pixel_hue(dim_t x, dim_t y, image_t hue) {
        set_pixel_hsv(x,y,hue,max_channel_val,max_channel_val);
      }

      inline void set_pixel_hue_scaled_data(dim_t x, dim_t y, double data_value) {
        set_pixel_hsv(x,y,scaled_hue(data_value,max_trans_val_),max_channel_val,max_channel_val);
      }

      template< typename data_t>
      inline void set_pixel_hue_scaled_data_or_special(dim_t x, dim_t y, data_t data_value,
                              data_t special_data_value, image_t special_grey_value) {
        if (data_value != special_data_value)
          set_pixel_hue_scaled_data(x,y,data_value);
        else
          set_pixel_grey(x,y,special_grey_value);
      }

      // max_channel_val means white
      template <typename vec_t>
      inline void draw_grey_path(vec_t x, vec_t y, image_t grey_value = max_channel_val) {
        for(size_t i=0; i< x.size(); ++i)
          set_pixel_grey(x[i],y[i],grey_value);
      }

      inline lodepng_error_t encode(std::vector<image_t>& png) {
        LodePNGInfo& info = png_state_.info_png;
        image_text_.add_text_to_state(info);
        return lodepng::encode(png, image_, width_, height_, png_state_);
      }
      
      inline void encode_and_write() {
        std::vector<image_t> png; // Don't make png a member, so that we release storage.
        //        lodepng_error_t error = lodepng::encode(png, image_, width_, height_);
        lodepng_error_t error = encode(png);
        if(!error) lodepng::save_file(png, filename_);
        else {
          std::cout << "DataImage, lodepng encoder error " << error << 
            ": "<< lodepng_error_text(error) << std::endl;
          abort();
        }
      }

      inline void encode_and_write(std::string& fname) {
        set_filename(fname);
        encode_and_write();
      }

      inline ImageText& text() { return image_text_;}

    private:
      std::vector<image_t> image_;
      dim_t height_ = 0;
      dim_t width_ = 0;
      std::string filename_; // output filename
      // maximum value of data to translate into hue (min is assumed zero)
      double max_trans_val_;
      lodepng::State png_state_;
      ImageText image_text_;
      
    }; /* END class DataImage  */

    /*** END class DataImage  *****************************************/

    /*   
     *   class FatDataImage. Each physical data site is fat_fac x fat_fac pixels in
     *   the image. We call these 'fatpixels'. We also draw thinner lines on the image.
     */
    class FatDataImage : public DataImage {
    public:

      inline void set_fat_fac(int fat_fac) { fat_fac_ = fat_fac;}

      /* Set image dimensions and also stash the "physical" dimensions. These
       will be retrieved when looping over physical sites. */
      inline void set_dims(dim_t h, dim_t w) override {
        DataImage::set_dims(fat_fac_*h,fat_fac_*w);
        fat_height_ = h;
        fat_width_ = w;
      }

      /* Set each pixel in one fatpixel to the same color */
      inline void set_channel(dim_t x, dim_t y, int ch, image_t val) override {
        dim_t xc = fat_fac_ * x;
        dim_t yc = fat_fac_ * y;
        dim_t bd = (fat_fac_-1)/2;
        for(dim_t x1=xc-bd; x1 <= xc+bd; ++x1)
          for(dim_t y1=yc-bd; y1 <= yc+bd; ++y1)
            DataImage::set_channel(x1,y1,ch,val);
      }

      enum path_direction_t { x_pos, x_neg, y_pos, y_neg};

      /* Set one color channel for all thin pixels in a line joining
       * centers of two fatpixels. The two fatpixels represent steps
       * in a path. x,y are final fatpixel coords, and dir is the
       * direction we came from.
      */
      inline void set_thin_cont_channel(dim_t x, dim_t y, path_direction_t dir, int ch, image_t val)  {
        dim_t xc = fat_fac_ * x;
        dim_t yc = fat_fac_ * y;
        if (dir == y_pos)
          for(dim_t y1=yc-fat_fac_; y1 <= yc; ++y1)
            DataImage::set_channel(xc,y1,ch,val);
        else if (dir == y_neg)
          for(dim_t y1=yc; y1 <= yc+fat_fac_; ++y1)
            DataImage::set_channel(xc,y1,ch,val);
        else if (dir == x_pos)
          for(dim_t x1=xc-fat_fac_; x1 <= xc; ++x1)
            DataImage::set_channel(x1,yc,ch,val);
        else  //  dir == x_neg
          for(dim_t x1=xc; x1 <= xc+fat_fac_; ++x1)
            DataImage::set_channel(x1,yc,ch,val);
      }

      inline void set_thin_channel(dim_t x, dim_t y, int ch, image_t val) {
        dim_t xc = fat_fac_ * x;
        dim_t yc = fat_fac_ * y;
        DataImage::set_channel(xc,yc,ch,val);
      }

      inline dim_t get_width() override { return fat_width_;}
      inline dim_t get_height() override { return fat_height_;}

      inline dim_t get_fat_width()  { return DataImage::get_width();}

      /* 
         Cannot figure out how to call base class methods and
         have them call base class methods, etc. So, we have to do it
         the ugly way. Entirely defeats the purpose of using derived class.
         "using DataImage::set_channel;" does not work! 
      */

      inline void set_thin_pixel_rgb(dim_t x, dim_t y, image_t r, image_t g, image_t b,
                                image_t alpha = max_channel_val) {
        set_thin_channel(x,y,0,r);
        set_thin_channel(x,y,1,g);
        set_thin_channel(x,y,2,b);
        set_thin_channel(x,y,3,alpha);        
      }

      inline void set_thin_cont_pixel_rgb(dim_t x, dim_t y, path_direction_t dir, image_t r, image_t g, image_t b,
                                image_t alpha = max_channel_val) {
        set_thin_cont_channel(x,y,dir,0,r);
        set_thin_cont_channel(x,y,dir,1,g);
        set_thin_cont_channel(x,y,dir,2,b);
        set_thin_cont_channel(x,y,dir,3,alpha);
      }

      inline void set_thin_cont_pixel_grey(dim_t x, dim_t y, path_direction_t dir, image_t grey_value) {
        set_thin_cont_pixel_rgb(x,y,dir,grey_value,grey_value,grey_value);
      }

      /* Draw a thin path. Set only pixel at center of each
       * fatpixel. The effect is a dotted line representing the
       * path. See draw_thin_grey_continuous_path below.
      */
      template <typename vec_t>
      inline void draw_thin_grey_path(vec_t x, vec_t y, image_t grey_value = max_channel_val)  {
        for(size_t i=0; i< x.size(); ++i)
          set_thin_pixel_rgb(x[i],y[i],grey_value,grey_value,grey_value);
      }

      /* Draw a path. Each step is a line of pixels through the fatpixel.
       * This gives a bit more information about the path than draw_thin_grey_path,
       * but also obscures more of the plot.
      */
      template <typename vec_t>
      inline void draw_thin_grey_continuous_path
      (vec_t x, vec_t y, image_t grey_value = max_channel_val)  {
        for(size_t i=0; i< x.size()-1; ++i) {
          path_direction_t dir;
          if ( y[i] < y[i+1]) dir = y_pos;
          else if ( y[i] > y[i+1]) dir = y_neg;
          else if ( x[i] < x[i+1]) dir = x_pos;
          else dir = x_neg;
          set_thin_cont_pixel_grey(x[i+1],y[i+1],dir,grey_value);
        }
      }
      
    private:
      dim_t fat_height_;
      dim_t fat_width_;
      int fat_fac_ = 3;

    }; /* END class FatDataImage  */
    /*** END class FatDataImage  *****************************************/


  } /* END namespace image */
} /* END namespace gjl */

#endif
