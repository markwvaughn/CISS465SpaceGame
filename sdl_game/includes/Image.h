/****************************************************************************
 Yihsiang Liow
 Copyright
****************************************************************************/
#ifndef IMAGE_H
#define IMAGE_H

#include "SDL.h"
#include "SDL_image.h"
//#include "SingletonSurface.h"
#include "Rect.h"

class Image
{
public:
    Image(const char * filename)
        : image(IMG_Load(filename))
    {
        if (image == NULL)
        {
            printf("Error in Image::Image(): No image file %s\n", filename);
            exit(1);
        }
        rect = image->clip_rect;
        //surface = SingletonSurface::getInstance();
    }
    
    Image(SDL_Surface * s) // Used by text surfaces
        : image(s)
    {
        rect = image->clip_rect;
    }

    Image(const Image & image_)
    {
        image = SDL_ConvertSurface(image_.image,
                                   image_.image->format,
                                   image_.image->flags);
        rect = image->clip_rect;
    }

    Image & operator=(const Image & image_)   
    {
        if (this == & image_)
            return *this;

        SDL_FreeSurface(image);
       
        image = SDL_ConvertSurface(image_.image,
                                   image_.image->format,
                                   image_.image->flags);
        
        return *this;
    }
    
    ~Image()
	{
        SDL_FreeSurface(image);
	}
      
    Rect getRect()
    {
        return rect;
    }
      
    //private:
    //Surface * surface;
    SDL_Surface * image;
    SDL_Rect rect;	
};

#endif

