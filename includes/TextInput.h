#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "DynamicText.h"

class TextInput
{
public:
    TextInput(Font & font,
              std::string str=" ",
              Color c=WHITE,
              float x=0, float y=0,
              int maxlen_=20,
              bool state_=true
             )                 
        : text(font, str, c, x, y), maxlen(maxlen_), state(state_)
    {
        SDL_EnableUNICODE(SDL_ENABLE);
    }

    ~TextInput()
    {
        SDL_EnableUNICODE(SDL_DISABLE);
    }
     
    void handle_input(const Event & event, Font & font)
    {
        std::string str = text.get_text();
        if(str == " ")
            str = "";
        int strlen = str.length();
    
        if (event.event.type == SDL_KEYDOWN)
        {
            Uint16 keypressed = event.event.key.keysym.unicode;
            std::string temp = str;
            
            if (strlen < maxlen &&
                (keypressed == (Uint16)' '
                 || (keypressed >= (Uint16)'0'
                     && keypressed <= (Uint16)'9')
                 || (keypressed >= (Uint16)'A'
                     && keypressed <= (Uint16)'Z')
                 || (keypressed >= (Uint16)'a'
                     && keypressed <= (Uint16)'z')
                 )
                )
                str += (char)keypressed;

            else if (keypressed == SDLK_BACKSPACE && strlen > 0)
            {
                if (strlen == 1) str = " ";
                else str.erase(strlen - 1);
            }

            else if (keypressed == SDLK_RETURN && strlen > 0)
            {
                state = false;
            }
        
            if(str != temp)
            {
                text.set_text(font, str);
            }
        }
    }
    
    void draw(Surface & surface)
    {
        text.draw(surface);
    }

    const DynamicText & get_text() const
    {
        return text;
    }

    const bool active() const
    {
        return state;
    }
    
private:
    DynamicText text;
    int maxlen;
    bool state;
};

#endif
