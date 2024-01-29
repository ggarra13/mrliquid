

mrLiquid
--------

mrLiquid has been in development for some time now.
Albeit there is some borrowed code in it from Colin Doncaster's Liquid 
Rendering Toolkit (mainly used in the camera, trims and some auxiliary 
functions), most of it is mine. 
Still, I thought mrLiquid was a fitting name for the project as liquid was, 
directly or indirectly, always an inspiration.  For those that don't know, 
liquid is another open source scene converter, which goes from maya to 
renderman (and a pretty damn good one, too).

mrLiquid was, I guess, more or less born out of two things:  a) my return to
production after a one year hiatus and b) a talk with Imre Tuske.  
We started discussing maya2mr and all the things we hated about it.  
My main goal was to have something better than the free maya2mr and with 
source code access.
Knowing the headaches that would lay ahead of me in this upcoming 
production, I decided to give it a shot and see how far I got on my own 
first.  Albeit I have some good coding background, writing a converter 
was never something I was very keen on doing so mrLiquid represents my 
first stab at it.
Funny enough, I found it to be quite easy to do and in 3 days I had a very 
solid converter up and many of the things that I thought were "hard" ended 
up not being so.
As I ended up developing this with no stand-alone mray, there may still be
some typos and problems here and there.

mrLiquid is based around the .mi scene description language, which is, in 
my opinion, a better design of an scene description language than 
Renderman's ribs are (albeit I consider ribs currently more production 
ready, as they are usually smaller due to their tokenized approch of binary
ribs and, if you are crazy enough, the rib stack can be used to shrink ribs
even more at the cost of being almost impossible to read or debug).
I do consider .mi scenes to be potentially much more user friendly to IPR 
and similar than ribs are, as I always found the rib stack to get in the way.
Just as a full disclosure, I have contributed somewhat to the design of the 
mental ray API, particularly in the past couple of years, so my pov is 
extremely biased.

With the advent of mental ray on pretty much every 3D package out there 
(which, I admit, was a surprise even to me), I thought having an open 
source converter is always a boon to vfx houses and the like.  It may also 
be helpful as a roadmap for other 3D packages that do not have the choice 
of renderers but may want to provide one.  It may also help other renderers 
that want to integrate themselves with Maya.

mrLiquid overall is currently based around trying to remain pretty 
compatible with maya2mr.  Of course, this is for now, as I expect to soon 
deviate as I add my own shit.   Users already familiar with a maya 
workflow can probably pick it up in no time and use it as a replacement for
maya2mr to spit out many of their scenes.

So... what's the difference with liquid? When I evaluated Colin's liquid, 
I thought it was brilliant, but I was disappointed in that it was hampered 
by the limitations imposed on the renderman spec and, thus, things such as 
rendertrees were not supported.  Providing support for this thru SL is 
entirely possible, as MTOR and Mayaman demonstrate, but it is quite a 
headache to do so right.  Also, concatenating .sl files on the fly is 
always a problem for providing fast feedback when shading connections change. 
Another thing I didn't like was that I had to once again re-learn a new
workflow.

mrLiquid fills that niche left by liquid in providing full support for 
shading networks and it would probably also be a good starting point for 
any converter to other non-renderman renderers (just change all the write* 
function calls to whatever you need for your own api).  I usually consider 
the sort of object oriented approach of .mi files (which are basically DAGs 
without the arrows) a really neat approach, as most (all?) 3d packages now 
work in similar fashion.  

Of course, .mi files are part of mental images' IP.  
So if you don't like getting attached to such a thing, feel free to use 
this as a base for your own stuff.  The syntax and api you use may be quite 
different, but I think you will find the scene, shading network traversal and 
dependency issues to be quite common among any renderer or 3d package.
Just make sure to remain somewhat compatible with something like this so 
I can plug your product in my pipeline easily if you later sell it 
back to me :)

mrLiquid is also a sort of a guilty pleasure of mine, as it was something I 
had been told I could not do for the past couple of years or that having a
pipeline with shading networks was useless or too much work.
If I had known I could write one in just two weeks I would have asked for
 a sickness leave :)

Another reason for mrLiquid was reassurance.  Alias + mray is
probably one of the best combos ever.  Something most of us have dreamt about
for years (as long as both programs remain as independent companies).  
Problem is... Alias has a bad precedent already.  You see... for those
of us that are old enough to remember, Alias some time ago already had a golden
chance to hook themselves to the best independent renderer.  Years ago, when
Alias swallowed TDI, they also swallowed the best or second best renderer on 
the market at the time (funny enough, it was similar to mray: great raytracer,
best shading networks, kick ass api, probably the first hybrid renderer, 
built-in IPR, etc).
Sadly, Alias, instead of killing their renderer and capitalizing on TDI's, 
did the exact opposite.  They killed all things TDI and stuck with rewriting
their Alias renderer for maya.
Today, I see Alias getting a second chance with mental ray.  The chance to do 
what they could have done 5+ years ago.  But... what can I say?  Once burnt,
twice learnt.  mrLiquid also is a reassurance (at least for me) that
regardless of what Alias does, my (personal) investment is safe.

One more reason was that I wanted mental images to go back to concentrate on 
improving their render core.  By having an open source converter, most
companies with even a minimum of R&D should be able to work around any
limitations of maya2mr.  I still do hope Alias finishes the IPR integration
of mray, thou.

The final reason for mrLiquid is that I want to see if it is possible to 
improve upon shading languages as we know them today and I need a base to 
start playing from.  Before you linch me, hear me out...
With the advent of gfx cards seen more and more as just another CPU, 
I am keen on having a single SIMD language that can be deployed everywhere, 
and not just for shading, which is, after all, just a very tiny 
subset of what vfx productions do or what they will want to do tomorrow.  
So far, all of the APIs I've seen (be it GL, Cg, mental ray's, NVidia's 
Gelato, RiSpec/RSL, BrookGPU, etc) have fallen quite short of what I would 
like.  
The only thing that does seem to be pretty much in tune with what I want is 
what the University of Waterloo is proposing in the form of libSh.
Not too crazy about the syntax and it is not quite a finished product yet, so 
we'll see...  perhaps combining two of the previous apis is still the best
approach.  I'm not TOO concerned with having to learn a new syntax, as I 
expect the language to cater more to programmers and technical TDs, while most
people would just benefit of the results thru shading networks or a wrapper
thru a more friendly language a la ruby.  Of course, since RSL has been around
for quite some time, deviating WAY too much is also probably not a good idea,
at least for the VFX community (games, cg movies, and others might not care so
much).


FEATURES
--------
- Primitives Supported:
	     - meshes (with and without holes)
	     - meshes (with subd approx.) - ie. traditional subds
	     - nurbs  (with and without trims)
	     - subds  (non-hierarchical for now)
	     Subds suffer from the 3/4 vertex limit of mray and require
	     a mray with mentalmatter for now.
- Light types supported:
	      - Ambient
	      - Point
	      - Spot
	      - Directional
	      - Area
	      - Volume
- Supports all mray approximations, including flagged approximations
- Supports instancing
- Supports incremental spitting of .mi files
- Motion blur, including multiple motion vectors
- Shading networks are supported (both maya's thru mayabase & 
  mray's custom shaders).
- Supports multiple render passes with incremental changes (ie. very small
  files), but not maya's render layers (which are crap, anyway).
- Supports customTexts in most places
- Supports user vectors
- Supports reference objects
- MEL and RUBY can be used as procedural languages to create data on the fly.
  (requires rubyMEL installed and loaded for ruby -- you can find it on 
  highend3d).  All customTexts can add the text as is, or interpret it as a 
  MEL or Ruby program, whose return value (NOT its output) is passed onto 
  the .mi file.
- Complex support of string parameter evaluation:
  * Supports evaluation of MEL variables in string parameters using a 
    ${VARIABLE} syntax.
  * Supports @,# evaluation in string parameters as replacement for frame
    number.  (ie.   test.@@@.pic  would become test.001.pic )
  * Also supports:
	 $PDIR  - project directory
	 $MIDIR - mi file directory
	 $SHMAP - shadow map directory
	 $PHMAP - photon map directory
	 $TEX	- texture directory
	 $NAME  - shader name
- Performance for spitting an .mi file out should be faster than maya2mr
  (expect a 20-50% performance boost, but I have not done true benchmarking
   on complex scenes yet and there's still some optimizing possible).
- Highly compatible with maya2mr's workflow
- Supports a very flexible and extremely efficient render pass system,
  using either MEL or Ruby as procedural languages.

( See TODO.txt for a list of features not yet supported and potentially 
upcoming features )


COMPATABILITY
-------------

mrLiquid was written with versions of mray3.2/3.3 or later in mind.
Previous versions of mray might also work but they may need minor source
code modifications (or some features may not work).
Future versions of mrLiquid will probably try to keep backwards compatability
with mray3.2 thru compile switches, at least for a while.
Note that maya shaders are usually also written with a particular version
of mray3.XX in mind, so be sure that you are using similar versions or
"Strange Things can Happen" (TM).


NOTES FOR DEVELOPERS
--------------------
mrLiquid should work relatively okay for any version of maya from 4.5 on, as
the API has not seen major modifications.  However, the mayabase shaderset
has suffered changes from version to version so not all things may end up
working in all maya versions.

On Windows
To compile, for versions previous to maya6 (5, 4.5, etc) it requires MSVC6.
For maya6 or later, it relies on MSVC7.
Note that due to API differences, maya5 or earlier may be missing features
(like light linking).

On Linux
To compile, you should be able to use gcc2.96 for versions previous to maya6.
And you should use gcc3.2 or later for versions for maya6 or later.
As I write this, code has not yet been tested under Linux, so there may still
be the need for changes in the mrLiquid source and no Makefile is yet 
provided.

mrLiquid uses a library for memory debugging when in debug mode.  
This library will usually report memory leaks on exit, mainly due to STL 
allocations on static elements which cannot be tracked.
If a true memory leak is present, you will likely see the line number
corresponding to it.


The future
----------
Who knows?  In an ideal world, someone would come up with my perfect tool 
and I would not have to touch a single line of code anymore.
But knowing how things usually go (and how much $$$ sometimes things cost), 
I expect to keep developing mrLiquid whenever I have the time.  
We'll see what happens...
I do have 2 or 3 new ideas going around in my mind... 


Thanks
------
To whoever knows me, to whoever I borrowed code from, to whoever taught 
me stuff, to whoever gave me a job, to friends and ailing parents.  
I'll eventually fill the list but it will probably be in another README :)  


License
-------
As my code still relies on parts of the Liquid Rendering Toolkit, I am more
or less obliged to release at least those parts of the code under the Mozilla 
Public License, which all in all is not bad.  

I still would prefer an even more free license like OpenBSD with no viral
copyleft in it.  
If you go thru the source code, you will see I have more or less clearly 
labeled each file that has partially borrowed liquid's code and is thus part 
of the MPL.

The contents of this file are subject to the Mozilla Public License 
Version 1.1 (the "License"); you may not use this file except in compliance 
with the License. You may obtain a copy of the License at 
http://www.mozilla.org/MPL/ 
 
Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
for the specific language governing rights and limitations under the License. 

