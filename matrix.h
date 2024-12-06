#ifndef matrix_h
#define matrix_h
#include <stdint.h>
#include "esphome/core/color.h"
#include "esphome/components/light/esp_hsv_color.h"
#include "esphome/components/light/light_color_values.h"
#include "esphome/components/light/addressable_light.h"
#include "lib8tion.h"

#define MTX_WIDTH 16
#define MTX_HEIGHT 16


//***************************************************************************************
//  EFFECTS ARRAYS
//***************************************************************************************

//For Fire
     static uint8_t heat[MTX_WIDTH * MTX_HEIGHT];

// for Fire12
     static  const uint8_t gamma8[] = {
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
              1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
              2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
              5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
              10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
              17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
              25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
              37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
              51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
              69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
              90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
              115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
              144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
              177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
              215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
            };

//***************************************************************************************
//  Matrix Class
//***************************************************************************************

class Matrix {
public:

            int mtx_zigzag; // pixel arrangement : 1 strip, 2 zigzag, 3 parallel   
            int mtx_type; // matrix type and orientation 1-8
            int mtx_width ;
            int mtx_height;

            float cosA;
            float sinA;
            float cur_hue;
            int cur_hue_int;

 Matrix () {
      mtx_type = 1;
      mtx_width =  16;
      mtx_height = 16;
      mtx_zigzag = 2;
      cosA = 0.0f;
      sinA = 0.0f;
      cur_hue = 0.0f;
      cur_hue_int = 0;
 }
 void setType (int mtx_t,int w, int h){
      mtx_type = min(8,max(0,mtx_t));
      mtx_width =  (mtx_type & 1) ? w : h;
      mtx_height = (mtx_type & 1)? h : w;
}
//***************************************************************************************
//  Get Pixel (Gyver Lamp) nahdles coordinates and rotation
//***************************************************************************************

///  Get Pixel number using X and Y coordinates
int getPix(int x, int y) {
  int thisX, thisY;
  switch (mtx_type) {
    case 1: thisX = x;                    thisY = y;                    break;
    case 2: thisX = y;                    thisY = x;                    break;
    case 3: thisX = x;                    thisY = (mtx_height - y - 1); break;
    case 4: thisX = (mtx_height - y - 1); thisY = x;                    break;
    case 5: thisX = (mtx_width - x - 1);  thisY = (mtx_height - y - 1); break;
    case 6: thisX = (mtx_height - y - 1); thisY = (mtx_width - x - 1);   break;
    case 7: thisX = (mtx_width - x - 1);  thisY = y;                    break;
    case 8: thisX = y;                    thisY = (mtx_width - x - 1);  break;
    default: thisX = x;                    thisY = y;                    break;
  }

  if ( !(thisY & 1) || (mtx_zigzag - 2) ) return (thisY * mtx_width + thisX);         // even line
  else return (thisY * mtx_width + mtx_width - thisX - 1);                            // odd line
}

//***************************************************************************************
//  Wrap (Gyver Lamp CPP)
//***************************************************************************************

uint8_t wrapX(int8_t x)
{
    return mod8(x + mtx_width, mtx_width );
}
uint8_t wrapY(int8_t y)
{
    return mod8(y + mtx_height ,mtx_height);
}

//***************************************************************************************
// Multiply 
//***************************************************************************************


uint8_t clampAndConvert(float v)
{
    if(v < 0.0f)
        return 0;
    if( v > 255.0f)
        return 255;
    return static_cast<uint8_t>(v);
}

Color TransformMultiply(
    const Color &in,  // color to transform
    const Color &cur  // tint color
) {
    float max; // value
    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    // float min = static_cast<float>(min  < in.b ? min  : in.b);
    float rx = (static_cast<float>(in.r)+static_cast<float>(cur.r))/512.0f * max;
    float gx = (static_cast<float>(in.g)+static_cast<float>(cur.g))/512.0f * max;
    float bx = (static_cast<float>(in.b)+static_cast<float>(cur.b))/512.0f * max;
    return Color(clampAndConvert(rx), clampAndConvert(gx), clampAndConvert(bx));

}


//***************************************************************************************
// HUE ROTATE           https://stackoverflow.com/a/11459607
//***************************************************************************************

float hue_matrix[3][3];

void SetHueRotation(float fHue){
  cosA = cos(fHue*3.14159265f/180); //convert degrees to radians
  sinA = sin(fHue*3.14159265f/180); //convert degrees to radians
    //calculate the rotation matrix, only depends on Hue
  hue_matrix[0][0] = cosA + (1.0f - cosA) / 3.0f;
  hue_matrix[0][1] = 1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[0][2] = 1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[1][0] = 1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[1][1] = cosA + 1.0f/3.0f*(1.0f - cosA);
  hue_matrix[1][2] = 1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[2][0] = 1.0f/3.0f * (1.0f - cosA) - sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[2][1] = 1.0f/3.0f * (1.0f - cosA) + sqrtf(1.0f/3.0f) * sinA;
  hue_matrix[2][2] = cosA + 1.0f/3.0f * (1.0f - cosA) ;
}

Color TransformHueShift(
    const Color &in  // color to transform
) 
{
    if (cur_hue == 0.0f) return Color(in.r,in.g,in.b);
    Color out = Color();
    //Use the rotation matrix to convert the RGB directly
    out.r = clampAndConvert(in.r*hue_matrix[0][0] + in.g*hue_matrix[0][1] + in.b*hue_matrix[0][2]);
    out.g = clampAndConvert(in.r*hue_matrix[1][0] + in.g*hue_matrix[1][1] + in.b*hue_matrix[1][2]);
    out.b = clampAndConvert(in.r*hue_matrix[2][0] + in.g*hue_matrix[2][1] + in.b*hue_matrix[2][2]);
    return out;
}


float rgb2hue(Color &in)
{
    uint8_t      min, max, delta;
    float hue = 0.0f;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    //out.v = max; // value    
    delta = max - min;
    if (delta == 0)
    {
        // h and s == 0
        return hue;
    }
    if( max > 0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        //out.s = (delta / max); // sauturation
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        return hue;
    }
    if  ( in.r >= max )                           // > is bogus, just keeps compilor happy
         hue = (float)( in.g - in.b ) / (float)delta;        // between yellow & magenta
    else if( in.g >= max )  
         hue = 2.0f + (float)( in.b - in.r ) / (float)delta;  // between cyan & yellow
    else hue = 4.0f + (float)( in.r - in.g ) / (float)delta;  // between magenta & cyan

    hue *= 60.0f;                              // degrees

    if( hue < 0.0f ) hue += 360.0f;

    return hue;
}

//***************************************************************************************
//  Fire 12
//***************************************************************************************

void Fire12_tick(AddressableLight *it){
  int Cooling = 256;
            int Sparking = 4; // num new sparks per frame
  
            uint8_t cooldown = 0;
            
            // Step 1. Cool down every cell a little
            for( int i = 0; i < it->size(); i++) {
              cooldown = static_cast<uint8_t>(random(0, ((Cooling * 10) / it->size()) + 2));
              
              if(cooldown>heat[i]) {
                heat[i]=0;
              } else {
                heat[i]=heat[i]-cooldown;
              }
            }
            
            // Step 2.  Heat from each cell drifts 'up' and diffuses a little
            
            for (int y= mtx_height - 1; y > 0;y--){
              for (int x = 0; x < mtx_width ;x++){
                switch (x){
                  case 0:
                  case 15:
                    heat[getPix(x,y)] = (heat[getPix(x,y-1)]+heat[getPix(wrapX(x-1),y-1)]+heat[getPix(wrapX(x+1),y-1)]) /3;
                    break;
                  default:
                    heat[getPix(x,y)] = (heat[getPix(x,y-1)]+heat[getPix(x-1,y-1)]+heat[getPix(x+1,y-1)])  /3;
                    break;
                }
              }
            }
            
            // Step 3.  Randomly ignite new 'sparks' near the bottom
            for ( int i = 0; i< Sparking ; i++) {
              int x = random( mtx_width );
              heat[getPix(x,0)] =  heat[getPix(x,0)]+random(160,255);
            }

            // Step 4. Apply color to pixels
            //
}


};


Matrix myMatrix;


#endif




