#ifndef MISC_H
#define MISC_H

// Convert input to string
template <class T>
std::string to_str(T input)
{
    std::ostringstream ret;
    ret << input;
    return ret.str();
}

#endif
