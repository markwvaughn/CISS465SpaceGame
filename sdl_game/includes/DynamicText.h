#ifndef DYNAMICTEXT_H
#define DYNAMICTEXT_H

#include "SDL_ttf.h"

class DynamicText
{
public:
    DynamicText(Font & font,
                std::string str=" ",
                Color c=WHITE,
                float x=0, float y=0
                )
        :text(str), color(c), image(NULL)
    {
        image = font.render_s(str.c_str(), color);
        rect = image->clip_rect;
        rect.x = x;
        rect.y = y;
    }
    
    DynamicText(const DynamicText & DT)
        :text(DT.text), color(DT.color), 
         image(DT.image), rect(DT.rect)
    {
        image = SDL_ConvertSurface(DT.image,
                                   DT.image->format,
                                   DT.image->flags);
    }
 
    DynamicText & operator=(const DynamicText & DT)   
    {       
        if (this == & DT)
            return *this;
        
        SDL_FreeSurface(image);

        text = DT.text;
        color = DT.color;
        image = SDL_ConvertSurface(DT.image,
                                   DT.image->format,
                                   DT.image->flags);
        rect = DT.rect;
        
        return *this;
    }
   
    ~DynamicText()
    {
        SDL_FreeSurface(image);
    }

    void draw(Surface & surface)
    {
        SDL_BlitSurface(image, NULL, surface.get(), &rect);
    }

    void set_text(Font & font, const std::string & str)
    {
        text = str;
        SDL_FreeSurface(image);
        image = font.render_s(str.c_str(), color);
        float x = rect.x;
        float y = rect.y;
        rect = image->clip_rect;
        rect.x = x;
        rect.y = y;
    }

    void set_color(const Color & color_)
    {
        color = color_;
    }

    void set_x(const float & x)
    {
        rect.x = x;
    }

    void set_y(const float & y)
    {
        rect.y = y;
    }

    std::string get_text() const
    {
        return text;
    }

    Color get_color() const
    {
        return color;
    }

    float get_x() const
    {
        return rect.x;
    }

    float get_y() const
    {
        return rect.y;
    }

    float get_w() const
    {
        return rect.w;
    }

    float get_h() const
    {
        return rect.h;
    }

    SDL_Rect get_rect() const
    {
        return rect;
    }

private:
    std::string text;
    SDL_Color color;
    SDL_Surface * image;
    SDL_Rect rect;
};

#endif
