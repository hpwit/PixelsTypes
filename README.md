
# Pixels library

## Introduction

### Why this library ?

While updating the led driver for the esp32 I had already created an abstraction of what a pixel is. Meaning for instance having RGB et RGBW object, in the code only manipulate a Pixel object that could be RGB or RGBW depending on choice upfront.

Plus on different forums I have noticed  the following :
1) code not working because of some out of bound assignation such as
```C
CRGB leds[NUM_LEDS];
....

leds[NUM_LEDS]=CRGB(255,0,0; <= this is not good :(
```

This is due to C++ not being memory safe (don't get me wrong I love c++)

  2) need for code like this
  ` leds[1,200]=leds[55,255]`
  or
  ` leds[1,200]=leds[255,55]. <= copy in reverse` 

3) need to  'split' a strip into several virtual strips
 I know there is the CRGBArray object bu I've personally never used it and it's really FastLED 'linked'

I wrote this library as an helper for non programmers as well as good ones that want to 'simplify' the code

⚠️ This library is not enough to drive pixels strips or panels, you will need to use your favorite led library for this (FastLED, WLED, NeoPixels, I2SClocklessLedDriver,...)

  ⓘ Even if their library is driver agnostic, for the examples I will use the FastLED. 
		 You will find the examples with I2SClocklessLedDriver in the library itself.
		 In the documentation I might write led or pixel interchangeably 


## Let's start

### The Pixel object

The `Pixel`  object is built the same way as the `CRGB` object in the FastLED library. 
in the following example the two objects are similar they both 'carry' the red, green, blue information of the led/pixel
```C
Pixel pix1=Pixel(255,2,0);
CRGB pix2=CRGB(255,2,0);
```
They are also compatible
```C
/*
the following #define is used to 'tell' that we want FastLED object compatibility
it also includes the FastLED library
*/
#define USE_FASTLED 
#include "pixelslib.h"

....
//now you can write this
Pixel pix=CRGB(255,4,7);
//or
Pixel leds[25];
leds[12]=CHSV(25,100,100);
```

The Pixel object can also represent other type of leds. For instance the RGBW leds
```C
#define USE_FASTLED
/*
This will 'tell' the library to use an extended version of the Pixel struct
*/ 
#define PIXEL_TYPE RGBW
#include "pixelslib.h"

Pixel pixrgbw=Pixel(255,47,25,35) //with the white component
//But you can still write this
Pixel pix=CRGB(255,4,7);
//or
Pixel leds[25];
leds[12]=CHSV(25,100,100)
/*
auto matically the following transformation is applied. to convert RGB=>RGBW
W = min(R, G, B);
R = R - W;
G = G - W;
B = B - W;
*/
```

At the moment of the writing of this library the following type are available
 - RGB (24 bits) default option
 - RGBW (32 bits)
 - APA102 or HD107 (32 bits)
 - HD108 (64 bits)


## Let's get serious

The usual way to declare a strip is the following

       CRGB leds[NUM_LEDS];

The `leds` variable represent an array or a collection of ordered leds/pixels. if I write the following:

```C
    CRGB leds[20];
    leds[0]=CRGB(255,0,0);
    leds[1]=CRGB(0,0,255);
    leds[2]=CRGB(0,255,0);
```
Then the first  led will be red, the second blue and the third green.

If you write `leds[21]=CRGB(89,45,21)` you might be in trouble for writing out of bound.

### The Pixels object

This is how to declare a Pixels object

    Pixels leds=Pixel(20):
ⓘ We will see alter that there are more ways of declaring Pixels but for the moment let's start with that

With `leds` as `Pixels` you can still do the following
```C
	Pixels leds=Pixel(20):
    leds[0]=CRGB(255,0,0);
    leds[1]=CRGB(0,0,255);
    leds[2]=CRGB(0,255,0);
    leds[3]=Pixel(255,255,12);
```

but `leds[56]=Pixel(45,47,48)` will not get you into trouble anymore :)
Indeed the Pixels object is not a simple array but a class. This allow to better control the behavior of the 'strip'
In that case the out of bound pixels doesn't get written 'somewhere' in memory hence avoiding crashing.

If the Pixels object would only do that, it will be limited. Let's dive into the functionalities

### Strip subset

You can now define a subset of the strip.
For instance if you have  a long strip of 250 leds and want to run an animation from led 45 to led 150 it can be hard to code. The Pixels class allows the creation of the subset

#### createSubset(int start, int length)

```C
	Pixels leds=Pixel(200):
	Pixels subset=leds.createSubset(45,105);

	subset[0]=Pixel(255,0,0);
	subset[25]=CRGB(0,255,0);
	subset[106]=Pixel(78,15,02); //this one will be out of bound od the subset hence not displayed	
```
What will we see in that case ?
 - `subset[0]=Pixel(255,0,0);` : the led 45 of the strip will turn red which is the  led 0 of the subset 
 - `subset[25]=Pixel(0,255,0);` : the led 70 of the strip will turn green which is the  led 25 of the subset
 - `subset[106]=Pixel(78,15,02)` : this one will be out of bound of the subset hence not displayed

It's like you have a new strip by itself

ⓘ  this works too `leds.createSubset(45,105)[4]=CRGB::Red`

 #### main functions of a Pixels object
 - `int getSize()` : give you the 'size' of the Pixels object
 - `void clear()`  : 'clear' a Pixels object
 - `void  copy(Pixels  ori)` : copy a subset within the current subset (example to follow)
 - `void fill(Pixel)` or `void fill(Pixels)` : fill the Pixels object
 - `void  setMapFunction(int (*fptr)(int  i, void  *args), void  *args, int  size)` : this one will be discussed in avance mapping

#### Subset of subset ?

As a subset is a Pixels class you an take the subset of a subset:
```C
	Pixels leds=Pixel(256);
	Pixels subset=leds.createSubset(0,100);
	Pixels sub_subset=subset.createSubset(50,16);
 ```
 It functionally works, so now you can have some fun, but be careful in doing subsets of subsets you need to keep track of where you are :)


#### overlapping subsets
Of course you can have overlapping subsets
```C
	Pixels leds=Pixel(256);
	Pixels subset1=leds.createSubset(0,100);
	Pixels subset2=leds.createSubset(50,16);
 ```

But you need to be careful  because for instance  `subset1[50]=subset2[0]`

#### let's change direction
Take the following example: you have a couple of strips arrange in a snake configuration.
You want to run an animation on an odd strip you need to 'mirror' the leds by code.

Running  this code :

```C
	Pixels leds=Pixel(256);
	Pixels subset1=leds.createSubset(0,16);
	Pixels subset2=leds.createSubset(16,16);
	
	subset2[0]=CRGB::Red;
	subset2[15]=CRGB::Blue;
	subset1[0]=CRGB::Yellow;
	subset1[15]=CRGB::Purple;
```
you get this

Indeed  `subset2[0]=leds[16] and subset[15]=leds[31]`
Now using this code
```C
	Pixels leds=Pixel(256);
	Pixels subset=leds.createSubset(0,16);
	Pixels reverse_subset=leds.createSubset(16,16,leddirection::BACKWARD);
	
	reverse_subset[0]=CRGB::Red;
	reverse_subset[15]=CRGB::Blue;
	subset[0]=CRGB::Yellow;
	subset[15]=CRGB::Purple;
```
 you get this
 

Which is more aligned with what you want. because now you have Indeed  `subset2[0]=leds[31] and subset[15]=leds[16]`

Another use case is to have one long led strip and you want an animation on both sides mirroring.

```C
	Pixels leds=Pixel(256);
	Pixels subset=leds.createSubset(0,128);
	Pixels reverse_subset=leds.createSubset(128,128,leddirection::BACKWARD);
	int off=0;
	
	while(1)
	{
		leds.clear();
		/*
		this would have had the same effect.
		subset.clear();
		reverse_subset.clear();
		*/
		for(int i=0;i<128;i++)
		{
			subset[i]=CHSV(i+off,255,255);
			reverse_subset[i]=CHSV(i+off,255,255);
		}
		FastLED.show();
		delay(50);
		off++;
	}
```

the different directions are

 - `FORWARD` :  default option
 - `BACKWARD` : will map the leds in reverse
 - `MAP` : will map the leds in a user defined way (we will see this later)

#### let's copy

As said in the introduction I see people wanted to write this: ` leds[1,200]=leds[55,255]` or ` leds[1,200]=leds[255,55]. <= copy in reverse` 

The Pixels Class allows this with the `copy` function.

 - `leds.copy(Pixels leds_ori)` : will try to copy the content of `leds_ori` in `leds`. 
 - `leds.copy(Pixels leds_ori,int length)`:will try to copy `length`  leds of `leds_ori` in `leds`. 
 - `leds.copy(Pixels leds_ori,leddirection dir)`: will try to copy the content of `leds_ori` in `leds` with respect of the direction. 
 - `leds.copy(Pixels leds_ori,int length,leddirection dir)` : will try to copy `length`  leds of `leds_ori` in `leds` with respect of the direction. 

In copy the default direction is `FORWARD` hence `leds.copy(Pixels leds_ori)=leds.copy(Pixels leds_ori,leddirection;;FORWARD)`

Let's take a look at our previous code with `copy` this time
```C
	Pixels leds=Pixel(256);
	Pixels subset=leds.createSubset(0,128);
	Pixels reverse_subset=leds.createSubset(128,128,leddirection::BACKWARD);
	int off=0;
	
	while(1)
	{
		leds.clear();
		/*
		this would have had the same effect.
		subset.clear();
		reverse_subset.clear();
		*/
		for(int i=0;i<128;i++)
		{
			subset[i]=CHSV(i+off,255,255);
		}
		reverse_subset.copy(subset);
		FastLED.show();
		delay(50);
		off++;
	}
```

The result is the same as before.

⚠️. double `BACKWARD`  = `FORWARD`
The example before work as intended despite the `copy` being `FORWARD`. This its because the strip being copied into is defined as `BACKWARD`
```C
  Pixels subset=leds.createSubset(0,128);
  Pixels reverse_subset=leds.createSubset(128,128,leddirection::BACKWARD);
  reverse_subset.copy(subset);
```
is the same as
```C
  Pixels subset=leds.createSubset(0,128);
  Pixels reverse_subset=leds.createSubset(128,128);
  reverse_subset.copy(subset,BACKWARD);
```
#### let's fill
The Pixels classs offer two fill capabilities:

 - `fill(Pixel pix)`: fill the set/subset with all the same color or `fill(CRGB pix)` if you're using FastLED
 - `fill((Pixels pattern)` : fill the set/subset with a pattern

```C
	Pixels leds=Pixel(256);
	Pixels subset1=leds.createSubset(0,128);
	Pixels subset2=leds.createSubset(128,128);
	Pixels pattern=Pixels(3);

    pattern[0]=CRGB::Red;
    pattern[1]=CRGB::Green;
    pattern[2]=CRGB::Blue;

	subset1.fill(pattern);
	subset2.fill(Pixel(255,0,0));
```

In that case the first part of the strip will be Red, Green, Blue repeating and the second part will be all Red.

## How do declare this in FastLED ?

All of this above is nice but I do I do it in FastLED full code ? How do I go from
`Pixels leds` to `CRGB leds[]` ?

Each `Pixels` Class  has a function that returns the pointer to the actual leds: `getPixels()`. As it returns a Pixel pointer it needs to be recast as a CRGB pointer for FastLED library as such `(CRGB *)leds.getPixels()`
Hence the following code :

```C
#define USE_FASTLED 
#include "pixelslib.h"
#define NUM_LEDS 256
#define DATA_PIN 4

Pixels leds=Pixels(NUM_LEDS);
Pixels subset=leds.createSubset(0,NUM_LEDS/2);
Pixels reverse_subset=leds.createSubset(NUM_LEDS/2,NUM_LEDS/2,leddirection::BACKWARD);
int off=0;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>((CRGB *)leds.getPixels(), NUM_LEDS);
  FastLED.setBrightness(10);
  while(1)
  {
    leds.clear();
    /*
    this would have had the same effect.
    subset.clear();
    reverse_subset.clear();
    */
    for(int i=0;i<128;i++)
    {
      subset[i]=CHSV(i+off,255,255);
    }
    reverse_subset.copy(subset);
    FastLED.show();
    delay(50);
    off++;
  }
}
```

## One strip is good how do I do this with several strips/pins ???

In this chapter I will describe how to declare in FastLED several strips over serveral pins.

### Using Subsets
```C
#define SIZE_STRIP1 45
#define SIZE_STRIP2 20
#define SIZE_STRIP3 80
#define SIZE_STRIP4 63

#define NUM_LEDS (SIZE_STRIP1+SIZE_STRIP2+SIZE_STRIP3+SIZE_STRIP4)

Pixels leds=Pixels(NUM_LEDS);
Pixels strip1=leds.createSubset(0,SIZE_STRIP1);
Pixels strip2=leds.createSubset(SIZE_STRIP1,SIZE_STRIP2);
Pixels strip3=leds.createSubset(SIZE_STRIP1+SIZE_STRIP2,SIZE_STRIP3);
Pixels strip4=leds.createSubset(SIZE_STRIP1+SIZE_STRIP2+SIZE_STRIP3,SIZE_STRIP4);

FastLED.addLeds<NEOPIXEL, DATA_PIN1>((CRGB *)strip1.getPixels(), SIZE_STRIP1);
FastLED.addLeds<NEOPIXEL, DATA_PIN2>((CRGB *)strip2.getPixels(), SIZE_STRIP2);
FastLED.addLeds<NEOPIXEL, DATA_PIN3>((CRGB *)strip3.getPixels(), SIZE_STRIP3);
FastLED.addLeds<NEOPIXEL, DATA_PIN4>((CRGB *)strip4.getPixels(), SIZE_STRIP4);
```

### Using Strips

You can define a Pixels class as a set of strips. each strip is also a Pixels objects. You can then access each strip using the `getStrip(int stripnum)` function.


```C
#define SIZE_STRIP1 45
#define SIZE_STRIP2 20
#define SIZE_STRIP3 80
#define SIZE_STRIP4 63

int lengths[4]={SIZE_STRIP1,SIZE_STRIP2,SIZE_STRIP3,SIZE_STRIP4};
Pixels leds=Pixels(lengths,4);
Pixels strip1=leds.getStrip(0);
Pixels strip2=leds.getStrip(1);
Pixels strip3=leds.getStrip(2);
Pixels strip4=leds.getStrip(3);

FastLED.addLeds<NEOPIXEL, DATA_PIN1>((CRGB *)strip1.getPixels(), SIZE_STRIP1);
FastLED.addLeds<NEOPIXEL, DATA_PIN2>((CRGB *)strip2.getPixels(), SIZE_STRIP2);
FastLED.addLeds<NEOPIXEL, DATA_PIN3>((CRGB *)strip3.getPixels(), SIZE_STRIP3);
FastLED.addLeds<NEOPIXEL, DATA_PIN4>((CRGB *)strip4.getPixels(), SIZE_STRIP4);
```
the following syntaxes would have also worked :

```C
FastLED.addLeds<NEOPIXEL, DATA_PIN1>((CRGB *)leds.getStrip(0).getPixels(), SIZE_STRIP1);
//or
FastLED.addLeds<NEOPIXEL, DATA_PIN2>((CRGB *)leds.getStrip(1).getPixels(), leds.getStrip(1).getSize());
```

As the strips are also Pixels objects you can write the following :
```C
Pixels leds=Pixels(lengths,4);
Pixels subsetOfStrip0=leds.getStrip(0).subset(10,40);
leds.getStrip(1).subset(1,20)[2]=Pixel(14,25,26);
leds.getStrip(2)[56]=Pixel(14,25,26);
...
```

## A bit of more advance stuff

Earlier I have written that there was some left over topics: `setMapFunction` and the `MAP` direction.

### WTH is that for ? let's take an example.

Let's go back to our 16x16 matrix in snake pattern. Let's define the lines.
```C
Pixels leds=Pixels(256);
Pixels line0=leds.createSubset(0,16);
Pixels line1=leds.createSubset(16,16,leddirection::BACKWARD);
```

defining them like this you can have line0[0] and line1[0] being write on top of each other. nut we can go further:
```C
Pixels leds=Pixels(256);
Pixels lines[16];
for (int i=0;i<16;i++)
   {
    lines[i]=leds.createSubset(i*16,16,i%2 ? leddirection::BACKWARD:leddirection::FORWARD);
    }
```
if you you put the first pixel  at the bottom, left, you can 'plot' a pixel at position (x,y) doing `lines[y][x]=CRGB(255,0,0);` 
What if you want to represent the columns of the matrix and wanted to write `columns[x][y]=CRBG(255,0,0);` ?
The usual way of creating subset will not work as the pixels are consecutive. You'll need a way to create a correspondance like this for the first column:
| position in subset   | Real position on led strip      | 
| ------------- | ------------- | 
| 0          | 0         | 
| 1           | 31         | 
| 2           | 32         |
| 3           | 63         |
| ...           | ...         |
| 14           | 224         |
| 15           | 255        |

for the first column the formula to 'map' *(here it goes)* the position in subset to the real position is
```C
int map(int pos)
{
	if(pos%2==0)
  {
    return pos*16;
  }
  else
  {
    return (pos+1)*16-1;
  }
}
```

the way to define this subset would be
 ```C
	Pixels leds=Pixels(256);
	Pixels column0=leds.createSubset(0,leddirection::MAP);
	column0.setMapFunction(map,NULL,0);
 ```

NB: we do not specify length in the  `createSubset` because its  `visual size` is the one of the parent Pixels object here `leds`

But this is for the first column only for the x column it would be :

```C
int map(int pos,int column)
{
	if(pos%2==0)
  {
    return pos*16+column;
  }
  else
  {
    return (pos+1)*16-1-column;
  }
}
```
We either write 16 mapping functions or we find a way to pass the column parameter. But I cannot write `column[x,i]` ...

here is the trick define a parameter structure in our case we just need one parameter (but we could imagine mapping functions with several parameters:

    struct parameters    
    {
      int column;
    } ;

here is the final code :
```C
 #define USE_FASTLED 
#include "pixelslib.h"
#define NUM_LEDS 256
#define DATA_PIN 4
struct parameters    
 {
   int column;
 } ;
Pixels leds=Pixels(256);
Pixels lines[16];
Pixels columns[16];

int map(int pos,void *params)
{
  parameters p=*(parameters *) params; 
  if(pos%2==0)
  {
    return pos*16+p.column;
  }
  else
  {
    return (pos+1)*16-1-p.column;
  }
}
    

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>((CRGB *)leds.getPixels(), NUM_LEDS);
  FastLED.setBrightness(100 );
  parameters params;
  for (int i=0;i<16;i++)
   {
     lines[i]=leds.createSubset(i*16,16,i%2 ? leddirection::BACKWARD:leddirection::FORWARD);
     columns[i]=leds.createSubset(0,leddirection::MAP);
     params.column=i;
     columns[i].setMapFunction(map,&params,sizeof(params));
    }
}

void loop() {
  for(int y=0;y<16;y++)
  {
    leds.clear();
    for(int x=0;x<16;x++)
    {
      lines[y][x]=CRGB(255,0,0);
      columns[y][x]=CRGB(255,0,0);
    }
    FastLED.show();
    delay(60);
  }
}
```
 ![Tux, the Linux mascot](/images/IMG_2744.jpg)
Couple of remarks:

 1. Your mapping function has to be declared as such: `int map(int   
    pos,void *params)` of course call it as you wish `int   
    itsgettingcrazy(int pos,void *params)` for instance, but the parameters need to stay the same. (the `*void` stuff is the way for C/C++ to pass object of unknown type)


 2. The `parameters p=*(parameters *) params` is the way to tell the code that the params of unknown type is actually of type `parameter` . You could have several mapping functions with different parameters.
 
```C
struct parameters1    
 {
   int column;
 } ;
 struct parameters2:    
 {
   int column;
   int PANEL_SIZE;
   float angle;
 } ;
 
int mapping1(int i, void *params)
{
	parameters1 p=*(parameters1*)params;
	//some mapping code
}

int mapping2(int i, void *params)
{
	parameters2 p=*(parameters2*)params;
	//some mapping code
}

parameters1 param1;
parameters2 param2;
subset1.setMapFunction(mapping1,&param1,sizeof(param1));
subset2.setMapFunction(mapping2,&param2,sizeof(param2));
```

### What its the point of all this ?

You could indeed do what I have done earlier with a simple map(X,Y) function, but imagine you're a bit drunk and the lines are not straight anymore ....
Indeed using the mapping you can have a line which is not straight like following a sinus curve
```C
int map2(int i,void * params)
{
  parameters p=*(parameters *) params;
 
  if (i<0 or i>15)
    return _OUT_OF_BOUND;
  int x1=i;
  int  y1=floor(10*sin(i*PI/2/8))+p.column;

  if(x1 >=16 or y1>=16 or x1<0 or y1<0)
    return _OUT_OF_BOUND;
  if(y1%2==0)
  {
    return x1+y1*16;
  }
  else
  {
    return y1*16+(16-x1-1);
  }
}

....
 for(int x=0;x<16;x++)
    {
      lines[y][x]=CRGB(255,0,0);
      columns[y][x]=CRGB(0,0,255);
      notstraight[y][x]=CRGB(0,255,0);
    }

```
despite writing everything 'straight' you have this :


 ![Tux, the Linux mascot](/images/IMG_2749.jpg)

maybe not really  magical but you can do text scrolling in weird directions:


[Scrolling  sinewavemapping](https://www.youtube.com/watch?v=xFmbf0tZU_8)


## Conclusion
I hope this library will help you in creating nice new animation and simply the use of long strips..
In any case drop a message if you're happy and if you're not, let me know how I can improve this