/**
 * @file   mrTextureStats.h
 * @author gga
 * @date   Sun Dec 10 23:18:52 2006
 * 
 * @brief  A class to handle texture statistics and histograms.
 *         Used by mrl_exr_file shader.
 * 
 */

#ifndef mrTextureStats_h
#define mrTextureStats_h


#ifndef SHADER_H
#  include "shader.h"
#endif


BEGIN_NAMESPACE( mr )

  //! Stores statistics about texture access
  struct TextureStats
  {
    //! Histogram of how often mipmap levals were called
    miUlong histogram[14];
    //! The number of texture misses
    miUlong numTextureMisses;
    //! The number of texture accesses
    miUlong numTextureAccesses;
    //! The total amount of texture data transmitted 
    miUlong transferredTextureData;
    //! The current number of textures
    miUlong numTextures;
    //! The peak number of textures
    miUlong numPeakTextures;
    //! The amount of memory at the peak devoted to textures
    miUlong peakTextureMemory;
    //! The number of times textures were flushed from memory
    miUlong textureFlushes;


  public:
    //! Initialize counters to 0
    inline void init()
    {
      for (int i = 0; i < 14; ++i )
	histogram[i] = 0;
      numTextures = 0;
      numTextureMisses = 0;
      numTextureAccesses = 0;
      transferredTextureData = 0;
      numPeakTextures = 0;
      peakTextureMemory = 0;
      textureFlushes = 0;
    }

    inline TextureStats()  {	init();  };
     
    inline ~TextureStats() {};

    //! Output statistics for texture access
    void stats()
    {
      miUlong misses = numTextureMisses - numPeakTextures;
      mi_info("--------------------------------------------------------");
      mi_info("                  Texture Statistics");
      mi_info("--------------------------------------------------------");
      mi_info("Total Texture Data:   % 12.2f Mb", 
	      (transferredTextureData/1024.0));
      double avg;
      if ( textureFlushes > 1 )
	{
	  avg  = (transferredTextureData-peakTextureMemory) / textureFlushes;
	  avg += peakTextureMemory;
	}
      else
	{
	  avg = transferredTextureData;
	}
      mi_info("Ideal Texture Memory: % 12.2f Mb", (avg / 1024.0));
      mi_info("Peak  Texture Memory: % 12.2f Mb", (peakTextureMemory/1024.0));
      mi_info("Texture Flushes:      % 12u", textureFlushes);
      mi_info("Peak Textures #:      % 12u", numPeakTextures);
      mi_info("Texture Accesses:     % 12u", numTextureAccesses);
      mi_info("Texture Block Misses: % 12u", misses);
      mi_info("--------------------------------------------------------");
      if ( numTextureAccesses == 0 ) return;

      float p[14];
      miUlong sum = 0;
      int i;
      for ( i = 0; i < 14; ++i )
	sum += histogram[i];
      if ( sum > 0 )
	{
	  for ( i = 0; i < 14; ++i )
	    p[i] = (float) histogram[i] / sum;
	  mi_info("               Texture Mip-Map Histogram");
	  mi_info("--------------------------------------------------------");
	  mi_info("%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|%.1f|"
		  "%.1f|%.1f|%.1f|", p[0], p[1], p[2],  p[3], 
		  p[4], p[5], p[6],  p[7], p[8], p[9], p[10], p[11], 
		  p[12], p[13] );
	}
      mi_info("--------------------------------------------------------");
    }

    //! Increment one level of the mipmap histogram.
    inline void level( int x )
    {
      if ( x > 13 ) x = 13;
      ++histogram[x];
    }
};

extern MR_LIB_EXPORT TextureStats* gStats;

END_NAMESPACE( mr )


#endif // mrTextureStats_h

