/**
 * @file   mrPerlinNoise.h
 * @author gga
 * @date   Wed Aug  1 23:08:47 2007
 * 
 * @brief  A simple Perlin noise implementation
 * 
 * 
 */

#ifndef mrPerlinNoise_h
#define mrPerlinNoise_h

void  noiseinit();
float noise(float x);
float noise(float x, float y);
float noise(float x, float y, float z);

#endif

