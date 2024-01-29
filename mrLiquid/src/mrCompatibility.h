/**
 * @file   mrCompatibility.h
 * @author gga
 * @date   Sat Dec 15 00:47:46 2007
 * 
 * @brief  some macros for backwards/forwards compatibility among
 *         maya versions
 * 
 * 
 */

#ifndef mrCompatibility_h
#define mrCompatibility_h

#if MAYA_API_VERSION >= 700
#  define MItDependencyNodes_thisNode( it ) it.thisNode();
#else
#  define MItDependencyNodes_thisNode( it ) it.item();
#endif

#if MAYA_API_VERSION >= 800
#  define MItDag_currentItem( it )  it.currentItem();
#else
#  define MItDag_currentItem( it )  it.item();
#endif


#endif // mrCompatibility_h
