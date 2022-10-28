
#ifdef USE_FASTLED
#include "FastLED.h"
#endif

#define _OUT_OF_BOUND -12
#define APA102 0
#define HD107 0
#define HD108 2
#define RGBW 4 
#define RGB 8 

#ifndef PIXEL_TYPE
    #define PIXEL_TYPE RGB
#endif

#if PIXEL_TYPE == RGBW

struct Pixel
{
    union
    {
        uint8_t raw[3];
        struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t white;
        };
    };

    inline Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t w) __attribute__((always_inline))
    : red(r), green(g), blue(b), white(w)
    {
        // brigthness =0xE0 |(br&31);
    }

    inline Pixel(uint8_t r, uint8_t g, uint8_t b) __attribute__((always_inline))
    : red(r), green(g), blue(b)
    {
        white = MIN(red, green);
        white = MIN(white, blue);
        red = red - white;
        green = green - white;
        blue = blue - white;
    }

    inline Pixel() __attribute__((always_inline))
    {
    }

#ifdef USE_FASTLED
    inline Pixel &operator=(const CRGB &rhs) __attribute__((always_inline))
    {

        red = rhs.r;
        green = rhs.g;
        blue = rhs.b;
        white = MIN(red, green);
        white = MIN(white, blue);
        red = red - white;
        green = green - white;
        blue = blue - white;
        return *this;
    }
#endif

    inline Pixel(const Pixel &rhs) __attribute__((always_inline))
    {
        // brigthness=rhs.brigthness;
        red = rhs.red;
        green = rhs.green;
        blue = rhs.blue;
        white = rhs.white;
    }
    inline Pixel &operator=(const uint32_t colorcode) __attribute__((always_inline))
    {
        // rgb colorg;
        red = (colorcode >> 24) & 0xFF;
        green = (colorcode >> 16) & 0xFF;
        blue = (colorcode >> 8) & 0xFF;
        white = colorcode & 0xFF;
        return *this;
    }
};
#endif

#if PIXEL_TYPE == RGB
struct Pixel
{
    union
    {
        uint8_t raw[3];
        struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };
    };

    inline Pixel(uint8_t r, uint8_t g, uint8_t b) __attribute__((always_inline))
    : red(r), green(g), blue(b)
    {
        // brigthness =0xE0 |(br&31);
    }

    inline Pixel() __attribute__((always_inline))
    {
    }

#ifdef USE_FASTLED
    inline Pixel &operator=(const CRGB &rhs) __attribute__((always_inline))
    {
        red = rhs.r;
        green = rhs.g;
        blue = rhs.b;
        return *this;
    }
#endif

    inline Pixel(const Pixel &rhs) __attribute__((always_inline))
    {
        // brigthness=rhs.brigthness;
        red = rhs.red;
        green = rhs.green;
        blue = rhs.blue;
    }
    inline Pixel &operator=(const uint32_t colorcode) __attribute__((always_inline))
    {
        // rgb colorg;
        red = (colorcode >> 16) & 0xFF;
        green = (colorcode >> 8) & 0xFF;
        blue = (colorcode >> 0) & 0xFF;
        return *this;
    }
};
#endif


#if PIXEL_TYPE == APA102

struct Pixel {
    union {
        uint8_t raw[4];
        struct 
        {
            uint8_t brigthness;
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            
        };
        
    };

    inline Pixel(uint8_t br,uint8_t r, uint8_t g,uint8_t b) __attribute__((always_inline))
    :red(r),green(g),blue(b)
{
    brigthness =0xE0 |(br&31);
}

	inline Pixel() __attribute__((always_inline)):brigthness(0xff)
    {

    }

       inline Pixel &operator= (const IndvBrightness& rhs) __attribute__((always_inline))
    {
         brigthness =0xE0 |(rhs.bred&31);      
        return *this;
    }
#ifdef USE_FASTLED
inline Pixel &operator= (const CRGB& rhs) __attribute__((always_inline))
    {
        red = rhs.r;
        green = rhs.g;
        blue = rhs.b;
        return *this;
    }
   #endif

inline Pixel (const Pixel& rhs) __attribute__((always_inline))
     {
         brigthness=rhs.brigthness;
         red=rhs.red;
         green=rhs.green;
         blue=rhs.blue;
     }
     inline Pixel& operator= (const uint32_t colorcode) __attribute__((always_inline))
    {
        red = (colorcode >> 16) & 0xFF;
        green = (colorcode >>  8) & 0xFF;
        blue = (colorcode >>  0) & 0xFF;
        return *this;
    }
        

};
#endif
#if PIXEL_TYPE == HD108

struct Pixel {
    union {
        uint16_t raw[4];
        struct 
        {
            uint16_t brigthness;
            uint16_t red;
            uint16_t green;
            uint16_t blue;
            
        };
        
    };

inline Pixel(uint8_t br,uint16_t r, uint8_t bg,uint16_t g,uint8_t bb,uint16_t b) __attribute__((always_inline))
    :red(r),green(g),blue(b)
{
    brigthness =0x8000 | ((br&31)<<10)|((bg&31)<<5) |(bb&31);
}

inline Pixel(uint16_t r, uint16_t g, uint16_t b) __attribute__((always_inline))
:red(r),green(g),blue(b),brigthness(0xffff) 
{
    //brigthness =0x8000 ;
}

	inline Pixel() __attribute__((always_inline)):brigthness(0xffff) 
    {
    }
     
     inline Pixel (const Pixel& rhs) __attribute__((always_inline))
     {
         brigthness=rhs.brigthness;
         red=rhs.red;
         green=rhs.green;
         blue=rhs.blue;
     }
#ifdef USE_FASTLED
	inline Pixel &operator= (const CRGB& rhs) __attribute__((always_inline))
    {
        red = rhs.r;
        green = rhs.g;
        blue = rhs.b;
        return *this;
    }
#endif


    inline Pixel &operator= (const IndvBrightness& rhs) __attribute__((always_inline))
    {
        brigthness =0x8000 | ((rhs.bred&31)<<10)|((rhs.bgreen&31)<<5) |(rhs.bblue&31);      
        return *this;
    }
     inline Pixel& operator= (const uint32_t colorcode) __attribute__((always_inline))
    {
         
        rgb16 color;
        color.r= (colorcode >> 16) & 0xFF;
        color.g = (colorcode >>  8) & 0xFF;
        color.b = (colorcode >>  0) & 0xFF;
       // Serial.printf(" %f %f %f \n",colorg.r,colorg.g,colorg.b);
        
         red=color.r<<8 +color.r;
         green=color.g<<8 +color.g;
         blue=color.b<<8 +color.b;
         
        brigthness=0xffff;
        return *this;
        
    }

    
};
#endif


enum class leddirection
{
    FORWARD,
    BACKWARD,
    MAP
};

class Pixels
{
public:
    inline Pixels() __attribute__((always_inline)) {}
    inline Pixels(const Pixels &rhs) __attribute__((always_inline))
    {
        _size = rhs._size;
        _direction = rhs._direction;
        _num_strips = rhs._num_strips;
        for (int i = 0; i < _num_strips; i++)
        {
            _sizes[i] = rhs._sizes[i];
        }
        ledpointer = rhs.ledpointer;
        mapFunction = rhs.mapFunction;

        // parent=rhs.parent;
    }
    Pixels(int size, Pixel *ledpoi)
    {
        Pixels(size, ledpoi, leddirection::FORWARD);
    }

    Pixels(int size, Pixel *ledpoi, leddirection direction)
    {
        __Pixels(size, ledpoi, direction, this);
    }

    void __Pixels(int size, Pixel *ledpoi, leddirection direction, Pixels *pib)
    {
        pib->_size = size;
        pib->ledpointer = ledpoi;
        pib->_num_strips = 0;
        pib->_direction = direction;
        //  pib->nb_child=0;
    }

    Pixels(int num_led_per_strip, int num_strips)
    {
        int sizes[16];
        for (int i = 0; i < num_strips; i++)
        {
            sizes[i] = num_led_per_strip;
        }
        __Pixels(sizes, num_strips, leddirection::FORWARD, this);
    }

    Pixels (int length)
    {
       int sizes[16];
        for (int i = 0; i < 1; i++)
        {
            sizes[i] = length;
        }
        __Pixels(sizes, 1, leddirection::FORWARD, this);
    }

    Pixels(int *sizes, int num_strips)
    {
        __Pixels(sizes, num_strips, leddirection::FORWARD, this);
    }

    Pixels(int *sizes, int num_strips, leddirection direction)
    {
        __Pixels(sizes, num_strips, direction, this);
    }
    void __Pixels(int *sizes, int num_strips, leddirection direction, Pixels *pib)
    {
        int size = 0;
        for (int i = 0; i < num_strips; i++)
        {
            size += sizes[i];
            pib->_sizes[i] = sizes[i];
        }

        pib->_num_strips = num_strips;

        ledpointer = (Pixel *)calloc(size, sizeof(Pixel));
        if (ledpointer == NULL)
        {
            pib->_size = 0;
        }
        else
        {
            pib->_size = size;
        }
        pib->_direction = direction;
    }
    Pixel &operator[](int i)
    {
        switch (_direction)
        {

        case (leddirection::FORWARD):
            if (i < 0)
                return offPixel;
            if (i >= _size)
                return offPixel;

            return *(ledpointer + i % _size);
            break;

        case (leddirection::BACKWARD):
            if (i < 0)
                return offPixel;
            if (i >= _size)
                return offPixel;
            return *(ledpointer + (_size - i % (_size)-1));
            break;

        case (leddirection::MAP):
            if (mapFunction)
            {
                int offset = mapFunction(i, arguments);
                // printf("%d %d\n",i,offset);
                if (offset == _OUT_OF_BOUND)
                {
                    return offPixel;
                }
                else
                    return *(ledpointer + (mapFunction(i, arguments) % _size));
            }

            else
                return *(ledpointer);
            break;
        default:
            return *(ledpointer);
            break;
        }
    }

    void copy(Pixels ori, int length)
    {
        copy(ori, length,leddirection::FORWARD);
    }

    void copy (Pixels ori, int length, leddirection dir)
    {
         leddirection ledd = _direction;
        if (_direction == leddirection::MAP)
            ledd = leddirection::FORWARD;
        if (length>ori._size)
        {
            length=ori._size;
        }
        for (int i = 0; i < length; i++)
        {
            if (dir == leddirection::FORWARD)
            {
                (*this)[i] = ori[i];
            }
            else
            {
                (*this)[i] = ori[ori._size - i - 1];
            }
        }
    }

    void copy(Pixels ori)
    {
        copy(ori, leddirection::FORWARD);
    }

    void copy(Pixels ori, leddirection dir)
    {
        copy(ori, ori._size,leddirection::FORWARD);
    }

    void fill(Pixel pix)
    {
        for(int i=0;i<_size;i++)
        {
            (*this)[i]=pix;
        }
    }
#ifdef USE_FASTLED
    void fill(CRGB pix)
    {
        Pixel pixel;
        pixel=pix;
        fill(pixel);
    }
#endif
    void fill(Pixels ori)
    {
        for(int i=0;i<_size;i++)
        {
            (*this)[i]=ori[i%ori._size];
        }
    }

    Pixels getStrip(int num_strip, leddirection direction)
    {
        if (_num_strips < 0 or _num_strips <= num_strip)
        {

            int d[0];
            return Pixels(d, 1, direction);
        }
        else
        {
            uint32_t off = 0;
            for (int i = 0; i < num_strip % _num_strips; i++)
            {
                off += _sizes[i];
            }

            return Pixels(_sizes[num_strip], ledpointer + off, direction);
        }
    }

    Pixels getStrip(int num_strip)
    {
        return getStrip(num_strip, leddirection::FORWARD);
    }

    int *getLengths()
    {
        return _sizes;
    }


    int getSize()
    {
        return _size;
    }

    int getNumStrip()
    {
        return _num_strips;
    }
    Pixel *getPixels()
    {
        return (Pixel *)ledpointer;
    }
    void clear()
    {
        memset(ledpointer, 0, _size * sizeof(Pixel));
    }

    Pixels createSubset(int start, int length)
    {
        return createSubset(start, length, leddirection::FORWARD);
    }

    Pixels createSubset(int start, leddirection direction)
    {
        if (start < 0)
            start = 0;
        return Pixels(_size, ledpointer + start, direction);
    }

    Pixels createSubset(int start, int length, leddirection direction)
    {
        if (start < 0)
            start = 0;
        if (length <= 0)
            length = 1;
        return Pixels(length, ledpointer + start, direction);
    }

    inline void setMapFunction(int (*fptr)(int i, void *args), void *args, int size)
    {
        mapFunction = fptr;
        if (arguments == NULL)
            arguments = (void *)malloc(sizeof(size));
        memcpy(arguments, args, size);
    }

private:
    Pixel *ledpointer;
    size_t _size = 0;
    int _sizes[16];
    int _num_strips = 0;
    leddirection _direction;
    void *arguments;
    int (*mapFunction)(int i, void *args);
    Pixel offPixel;
};