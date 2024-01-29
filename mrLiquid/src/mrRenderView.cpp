//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

// this include must come first due to windows winsock2 crap
#include "mrSocket.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <limits>
#include <string>
#include <cmath>
#include <cassert>

#include "maya/MRenderView.h"
#include "maya/M3dView.h"
#include "maya/MDagPath.h"
#include "maya/MImage.h"

#ifndef mrIO_h
#include "mrIO.h"
#endif

#ifndef mrDebug_h
#include "mrDebug.h"
#endif

#ifndef mrPipe_h
#include "mrPipe.h"
#endif

#ifndef mrThread_h
#include "mrThread.h"
#endif

#ifndef mrStandalone_h
#include "mrStandalone.h"
#endif

#ifndef mrTranslator_h
#include "mrTranslator.h"
#endif

#include "mrRenderView.h"
#include "mrByteSwap.h"


extern MString tempDir;

#undef max // oh, you have to love windows...


#ifndef LPHOSTENT
#define LPHOSTENT   hostent*
#define SOCKET      int
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR    sockaddr
#define LPSOCKADDR  sockaddr*
#define LPIN_ADDR   in_addr*
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR   -1
#endif


// // Helper macro for displaying errors
// #define LOG_ERROR(s)	MRL_FPRINTF(stderr,"\n%s: %d\n", s, WSAGetLastError())


void socket_read( mrRenderView::stubData* stub, 
                  const char* server, const int port )
{
  using namespace std;
  printf("\nStream Client connecting to server: %s on port: %d\n",
	 server, port);

  //
  // Find the server
  //
  LPHOSTENT lpHostEntry;

  lpHostEntry = gethostbyname(server);
  if (lpHostEntry == NULL)
    {
      LOG_ERROR("gethostbyname()");
      return;
    }

  //
  // Create a TCP/IP stream socket
  //
  SOCKET	theSocket;

  theSocket = socket(AF_INET,			// Address family
		     SOCK_STREAM,		// Socket type
		     IPPROTO_TCP);		// Protocol
  if (theSocket == INVALID_SOCKET)
    {
      LOG_ERROR("socket()");
      return;
    }

  //
  // Fill in the address structure
  //
  SOCKADDR_IN saServer;

  saServer.sin_family = AF_INET;
  saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
  // Server's address
  saServer.sin_port = htons(port);  // Port number from command line

  //----------------------
  // Bind the socket.
  if (bind( theSocket, 
	    (SOCKADDR*) &saServer, 
	    sizeof(saServer)) == SOCKET_ERROR) 
    {
      printf("bind() failed.\n");
      closesocket(theSocket);
      return;
    }

  //
  // connect to the server
  //
  int nRet = connect(theSocket,	       // Socket
		     (LPSOCKADDR)&saServer,   // Server address
		     sizeof(struct sockaddr));// Length of server 
  // address struct
  if (nRet == SOCKET_ERROR)
    {
      LOG_ERROR("socket()");
      closesocket(theSocket);
      return;
    }

  //
  // Send data to the server
  //
#define BUF_LEN 256
  char szBuf[BUF_LEN+1];

  strcpy(szBuf, "stream_begin 0\n");
  nRet = send(theSocket,				// Connected socket
	      szBuf,					// Data buffer
	      strlen(szBuf),			// Length of data
	      0);						// Flags
  if (nRet == SOCKET_ERROR)
    {
      LOG_ERROR("send()");
      closesocket(theSocket);
      return;
    }


  bool stop = false;
  int width, height;

  while ( stop == false )
    {

      //
      // Wait for a reply
      //
      nRet = recv(theSocket,	// Connected socket
		  szBuf,	// Receive buffer
		  BUF_LEN,	// Size of receive buffer
		  0);		// Flags
      if (nRet == SOCKET_ERROR || nRet > BUF_LEN)
	{
	  break;
	}
      szBuf[nRet] = 0;


      std::istringstream parser( szBuf );

      std::string cmd;
      while ( std::getline(parser, cmd, ' ') )
	{
	  int xl, yl, xh, yh;
	  if ( cmd == "rect_begin:" )
	    {
	      parser >> xl >> xh >> yl >> yh;
	      DBG( "RECT BEGIN: " << xl << "," << yl << "-" << xh << "," << yh );
	      // ignore the rest of the line (which should be empty really)
	      parser.ignore(std::numeric_limits<streamsize>::max(), '\n');
	      continue;
	    }
	  else if ( cmd == "frame_begin:" )
	    {
	      int fb;
	      parser >> fb; // frame buffer #
	      parser >> width;
	      parser >> height;
	      DBG( "FRAME BEGIN: " 
		   << fb << " " << width << ", " << height );
	      // ignore the rest of the line (which should be empty really)
	      parser.ignore(std::numeric_limits<streamsize>::max(), '\n');

	      MRenderView::startRender(width, height, true, true);
	      continue;
	    }
	  else if ( cmd == "rect_data:" )
	    {
	      char c;
	      int size, fb, fbtype;
	      short comps, bits;
	      parser >> size >> c;
	      parser >> xl >> yl >> xh >> yh >> c;
	      parser >> fb >> fbtype >> c;
	      parser >> width >> height >> comps >> bits;
	      parser.ignore(std::numeric_limits<streamsize>::max(), '\n');

	      DBG( "RECT_DATA: " << size << endl
		   << xl << "," << yl << "-" << xh << "," << yh << endl
		   << "fb: " << fb << " type:" << fbtype << endl
		   << "W,H: " << width << ", " << height << endl
		   << "comps: " << comps << " bits:" << bits );

	      int w = xh - xl + 1;
	      int h = yh - yl + 1;
	      unsigned char* bytes = new unsigned char[size];

	      int pos = parser.tellg();
	      assert( pos >= 0 );

	      int read = nRet - pos;
	      if ( read > 0 )
		{
		  assert( read < BUF_LEN-pos );
		  memcpy( bytes, szBuf + pos, read);
		}
	      else
		{
		  read = 0;
		}

	      unsigned char* rest = bytes + read;
	      read = size - read;

	      int total = read;
	      int sum   = 0;

	      while ( sum < total )
		{
		  int data = recv(theSocket,	// Connected socket
				  (char*)rest,	// Receive buffer
				  read,	// Size of receive buffer
				  0);	// Flags
		  if (data == SOCKET_ERROR)
		    {
		      LOG_ERROR("recv()");
		      stop = true;
		      break;
		    }
		  sum  += data;
		  rest += data;
		  read -= data;
		}

	      if ( sum != total )
		{
		  LOG_ERROR( "WRONG DATA SIZE RECEIVED." );
		  stop = true;
		}
	      if ( stop ) break;


#ifdef DEBUG
	      unsigned char* last = bytes + size;
#endif
	      RV_PIXEL* pixels = new RV_PIXEL[w*h];

	      switch( bits )
		{
		case 1:
		  {
		    unsigned char* m = bytes;
		    for ( int y = 0; y < h; ++y )
		      {
			for ( int x = 0; x < w; ++x, ++m )
			  {
			    RV_PIXEL& p = pixels[ y*w + x ];
			    p.r = *m ? 255.0f : 0.0f;
			    if ( comps > 1 ) ++m;
			    p.g = *m ? 255.0f : 0.0f;
			    if ( comps > 2 ) ++m;
			    p.b = *m ? 255.0f : 0.0f;
			    if ( comps > 3 ) 
			      {
				++m;
				p.a = 255.0f * *m;
			      }
			    else
			      {
				p.a = 255.0f;
			      }
			  }
		      }
		    break;
		  }
		case 8:
		  {
		    float val;
		    unsigned char* m = bytes;
		    for ( int y = 0; y < h; ++y )
		      {
			for ( int x = 0; x < w; ++x, ++m )
			  {
			    RV_PIXEL& p = pixels[ y*w + x ];

			    val = *m / 255.0f;
			    val = powf( val, stub->gamma );
			    p.r = 255.0f * val;

			    if ( comps > 1 ) ++m;
			    val = *m / 255.0f;
			    val = powf( val, stub->gamma );
			    p.g = 255.0f * val;

			    if ( comps > 2 ) ++m;
			    val = *m / 255.0f;
			    val = powf( val, stub->gamma );
			    p.b = 255.0f * val;

			    if ( comps > 3 ) 
			      {
				++m;
				val = *m / 255.0f;
				val = powf( val, stub->gamma );
			      }
			    else
			      {
				val = 1.0f;
			      }
			    p.a = 255.0f * val;
			  }
		      }
		    break;
		  }
		case 16:
		  {
		    float val;
		    unsigned short* m = (unsigned short*) bytes;
		    for ( int y = 0; y < h; ++y )
		      {
			for ( int x = 0; x < w; ++x, ++m )
			  {
			    RV_PIXEL& p = pixels[ y*w + x ];
			    *m = ntohs( *m );
			    val = *m / 65535.0f;
			    val = powf( val, stub->gamma );
			    p.r = 255.0f * val;
			    if ( comps > 1 ) ++m;

			    *m = ntohs( *m );
			    val = *m / 65535.0f;
			    val = powf( val, stub->gamma );
			    p.g = 255.0f * val;

			    if ( comps > 2 ) ++m;
			    *m = ntohs( *m );
			    val = *m / 65535.0f;
			    val = powf( val, stub->gamma );
			    p.b = 255.0f * val;

			    if ( comps > 3 ) 
			      {
				++m;
				*m = ntohs( *m );
				val = *m / 65535.0f;
			      }
			    else
			      {
				val = 1.0f;
			      }
			    p.a = 255.0f * val;
			  }
		      }
		    break;
		  }
		case 32:
		  {
		    float val;
		    float* m = (float*) bytes;
		    for ( int y = 0; y < h; ++y )
		      {
			for ( int x = 0; x < w; ++x, ++m )
			  {
			    RV_PIXEL& p = pixels[ y*w + x ];

			    MAKE_BIGENDIAN( *m );
			    val = powf( *m, stub->gamma );
			    p.r = 255.0f * val;

			    if ( comps > 1 ) ++m;
			    MAKE_BIGENDIAN( *m );
			    val = powf( *m, stub->gamma );
			    p.g = 255.0f * val;

			    if ( comps > 2 ) ++m;
			    MAKE_BIGENDIAN( *m );
			    val = powf( *m, stub->gamma );
			    p.b = 255.0f * val;

			    if ( comps > 3 ) 
			      {
				++m;
				MAKE_BIGENDIAN( *m );
			      }
			    else
			      {
				val = 1.0f;
			      }
			    p.a = 255.0f * val;
			  }
		      }
		    break;
		  }
		default:
		  LOG_ERROR("Unknown bit depth");
		  stop = true;
		  break;
		}
	      MRenderView::updatePixels( xl, xh, yl, yh, pixels );
	      MRenderView::refresh( xl, xh, yl, yh );
	      delete [] pixels;
	      delete [] bytes;
	      break;
	    }
	  else if ( cmd == "rect_end:" )
	    {
	      // ignore the rest of the line
	      parser.ignore(std::numeric_limits<streamsize>::max(), '\n');
	    }
	  else if ( cmd == "frame_end:\n" )
	    {
	      MRenderView::endRender();
	      parser.ignore(std::numeric_limits<streamsize>::max(), '\n');
	      break;
	    }
	  else 
	    {
	      MString msg = "Unknown command '";
	      msg += cmd.c_str();
	      msg += "'";
	      LOG_ERROR(msg);
	      stop = true;
	      break;
	    }
	}
    }

  closesocket(theSocket);
}


/** 
 * Callback function for a new thread to read from a MRL_FILE descriptor
 * and send messages to maya console.
 * 
 * @param v a mrTranslator* (cast to void*)
 * 
 * @return 0
 */
MR_THREAD_RETURN mr_stub_reader_cb( void* v )
{
   char  line[1024];
   line[1023] = 0;

   if ( tempDir == "" )
   {
      tempDir = getenv("TEMP");
      if ( tempDir == "" )
      {
	 tempDir = getenv( "TEMPDIR");
      }
      if ( tempDir == "" )
      {
	 tempDir = getenv( "TMP");
      }
      if( tempDir == "" ) tempDir = "/usr/tmp/";
#define ADD_SLASH(x) \
  if ( x.rindex('/') != ((int)x.length()) - 1 ) x += "/";
      ADD_SLASH( tempDir );
   }


   cerr << "stub reading thread started" << endl;

   MString file = tempDir + "ipr.exr";
   struct stat sbuf;

   while (1)
   {
      int result = STAT( file.asChar(), &sbuf );
      if ( result != 0 )
      {
	 MString msg = "No \"" + file + "\"."; 
	 LOG_ERROR(msg);
	 MR_THREAD_EXIT(1);
      }

      int width;
      int height;

      if ( sbuf.st_size == 128 )
      {
	 // ...stub... read line
	 char mray[14];
	 char server[128];
	 int socket1;
	 float gamma;
	 int pid;
	 int socket2 = 0;
	 char c;

	 std::ifstream stub( file.asChar() );
	 stub.getline( mray, 14, ',');
	 stub >> width >> c;
	 stub >> height >> c;
	 stub.getline( server, 128, ',');
	 stub >> socket1 >> c;
	 stub >> gamma >> c;
	 stub >> pid >> c;
	 stub >> socket2;
	 stub.close();

#if defined(WIN32) || defined(WIN64)
	 // Initialize winsock
	 WORD wVersionRequested = MAKEWORD(1,1);
	 WSADATA wsaData;
	 int nRet;

	 nRet = WSAStartup(wVersionRequested, &wsaData);
	 if (wsaData.wVersion != wVersionRequested)
	 {
	    MRL_FPRINTF(stderr,"\n Wrong winsock version\n");
	    MR_THREAD_EXIT(1);
	 }
#endif

	 short port = socket2;
	 if ( port <= 0 ) port = socket1;
	 mrRenderView::stubData* stubdata = (mrRenderView::stubData*) v;
	 socket_read( stubdata, server, port );

#if defined(WIN32) || defined(WIN64)
	 // release winsock
	 WSACleanup();
#endif
	 break;
      }
   }

   MR_THREAD_EXIT(0);
}

////////////////// IMGPIPE ////////////////////////////////////////////////////

//
// Struct used to hold mray's pipe packets (as described by thomas at LAmrUG)
//
struct mrPipePacket
{
     int type;
     union {
	  int frame;
	  int width;
	  int xl;
     };
     union {
	  int height;
	  int xh;
     };
     union {
	  float gamma;
	  int   yl;
     };
     int yh;
};

/** 
 * Callback function for a new thread to read from a MRL_FILE descriptor
 * and send image to maya's render view.
 * 
 * @param v a mrTranslator* (cast to void*)
 * 
 * @return 0
 */
MR_THREAD_RETURN mr_pipe_reader_cb( void* v )
{
   char  line[1024];
   line[1023] = 0;

   mrTranslator* t = (mrTranslator*) v;
   FILE* f = t->pipeHandle();

   int width;
   int height;

   mrPipePacket p;
   while( fread( &p, sizeof(mrPipePacket), 1, f ) )
   {
      p.type = ntohl( p.type );
      p.xl   = ntohl( p.xl );
      p.yl   = ntohl( p.yl );
      p.xh   = ntohl( p.xh );
      p.yh   = ntohl( p.yh );

      switch( p.type )
      {
	 case 4:
            MRenderView::endRender();
	    continue;
	 case 5:
// 	    if (! MRenderView::doesRenderEditorExist() )
// 	    {
// 	       continue;
// 	    }
	    width  = p.width;
	    height = p.height;
//             MRenderView::startRender(width, height, false, true);
//             MRenderView::startRender(width, height, true, true);
            MRenderView::startRegionRender(width, height, 0, width-1,
                                           0, height-1, true, true);
	    continue;
	 case 6:
	    // new frame
	    continue;
	 case 2:
	    // tile data
	    {
	       int w = p.xh - p.xl + 1;
	       int h = p.yh - p.yl + 1;
	       int size = w*h;
	       char*     mental = new char[size*4];
	       RV_PIXEL* pixels = new RV_PIXEL[size];

	       fread( mental, sizeof(char), size*4, f );
	       char* m     = mental;
	       for ( int y = p.yl; y <= p.yh; ++y )
	       {
		  for ( int x = p.xl; x <= p.xh; ++x )
		  {
                     RV_PIXEL& c = pixels[ (y-p.yl)*w + (x-p.xl) ];
		     c.r = *m++;
		     c.g = *m++;
		     c.b = *m++;
		     c.a = *m++;
		  }
	       }
	       MRenderView::updatePixels( p.xl, p.xh, p.yl, p.yh, pixels );
	       MRenderView::refresh( p.xl, p.xh, p.yl, p.yh );
	       delete [] pixels;
	       delete [] mental;
	    }
	    continue;
	 case 3:
	    // refuse connection -- exit
	    LOG_ERROR("Connection refused");
            MRenderView::endRender();
	    MR_THREAD_EXIT(0);
	    break;
	 default:
	    LOG_ERROR("MRenderView unknown packet type");
	    continue;
      }
   }

//    MGlobal::executeCommand("mentalIPRStop");

   MR_THREAD_EXIT(0);
}



/** 
 * Callback function for a new thread to read from a MRL_FILE descriptor
 * and image to the current 3d view.
 * 
 * @param v a mrTranslator* (cast to void*)
 * 
 * @return 0
 */
MR_THREAD_RETURN mr_pipe_view_reader_cb( void* v )
{
   char  line[1024];
   line[1023] = 0;

   mrTranslator* t = (mrTranslator*) v;
   FILE* f = t->pipeHandle();

   int width  = 0, height = 0;

   mrPipePacket p;
   while( fread( &p, sizeof(mrPipePacket), 1, f ) )
   {
      p.type = ntohl( p.type );
      p.xl   = ntohl( p.xl );
      p.yl   = ntohl( p.yl );
      p.xh   = ntohl( p.xh );
      p.yh   = ntohl( p.yh );

      switch( p.type )
      {
	 case 4:
	    continue;
	 case 5:
	    {
	      width  = p.width;
	      height = p.height;

	      // M3dView  view  = M3dView::active3dView();
	      // 	    int pw = view.portWidth();
	      // 	    int ph = view.portHeight();
	      
	      // 	    cerr << "Viewport: " << pw << "-" << ph << endl;
	    }
	    continue;
	 case 6:
	    // new frame
	    continue;
	 case 2:
	    // tile data
	    {
	       M3dView  view  = M3dView::active3dView();
	       int vw = view.portWidth();
	       int vh = view.portHeight();

	       int w = p.xh - p.xl + 1;
	       int h = p.yh - p.yl + 1;
	       int size = w*h*4;
	       char*          mental = new char[size];
	       fread( mental, sizeof(char), size, f );

	       unsigned char* pixels = new unsigned char[size];

	       MImage image;
	       image.create(w, h);

	       for ( int i = 0; i < size; ++i )
		  pixels[i] = mental[i];
	       image.setPixels( pixels, w, h );

// 	       float wpercent = (float) vw / (float) width;
// 	       float hpercent = (float) vh / (float) height;

	       float xl = (float) p.xl / (float) (width-1);
	       float yl = (float) p.yl / (float) (height-1);
	       float xh = (float) p.xh / (float) (width-1);
	       float yh = (float) p.yh / (float) (height-1);

	       int ixl = (int) floor(vw * xl + 0.5f);
	       int iyl = (int) floor(vh * yl + 0.5f);
	       int ixh = (int) floor(vw * xh + 0.5f);
	       int iyh = (int) floor(vh * yh + 0.5f);

	       int wv = ixh - ixl + 1;
	       int hv = iyh - iyl + 1;
	       image.resize( wv, hv ); 

// 	       cerr << "x,y: " << xl << ", " << yl
// 		    << " - " << xh << "," << yh 
// 		    << " w,h:" << wv << "," << hv << endl;
 	       view.beginGL();

// 	glPushAttrib( GL_ALL_ATTRIB_BITS ); 

// #ifdef _WIN32
// 	SwapBuffers( view.deviceContext() );
// #elif defined (OSMac_)
// 	::aglSwapBuffers(view.display()); 
// #else
// 	glXSwapBuffers( view.display(), view.window() );
// #endif

	       view.writeColorBuffer( image, ixl, iyl );

#ifdef _WIN32
	SwapBuffers( view.deviceContext() );
#elif defined (OSMac_)
	::aglSwapBuffers(view.display()); 
#else
	glXSwapBuffers( view.display(), view.window() );
#endif
// 	glPopAttrib(); 

 	       view.endGL();

 	       delete [] pixels;
	       delete [] mental;
	    }
	    continue;
	 case 3:
	    // refuse connection -- exit
	    LOG_ERROR("Connection refused");
	    MR_THREAD_EXIT(0);
	    break;
	 default:
	    LOG_ERROR("mr3dView unknown packet type");
	    continue;
      }
   }

   MR_THREAD_EXIT(0);
}


bool mrRenderView::iffstub_reader( mrRenderView::stubData* stub )
{
  MR_THREAD hThread;
  return mr_new_thread( hThread, mr_stub_reader_cb, stub );
}


bool mrRenderView::imgpipe_reader( mrTranslator* f )
{
  MR_THREAD hThread;
  return mr_new_thread( hThread, mr_pipe_reader_cb, f );
}

bool mr3dView::imgpipe_reader( mrTranslator* f )
{
  MR_THREAD hThread;
  return mr_new_thread( hThread, mr_pipe_view_reader_cb, f );
}



#ifdef MI_RELAY
#include "raylib/mrRenderView.inl"
#endif
