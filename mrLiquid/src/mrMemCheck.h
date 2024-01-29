/*
**
** The contents of this file are subject to the Mozilla Public License Version 1.1 (the 
** "License"); you may not use this file except in compliance with the License. You may 
** obtain a copy of the License at http://www.mozilla.org/MPL/ 
** 
** Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
** WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
** language governing rights and limitations under the License. 
**
** The Original Code is called mrLiquid. 
** 
** The Initial Developer of the Original Code is Gonzalo Garramuno.
** 
*/

#ifndef mrMemCheck_h
#define mrMemCheck_h

#ifdef MR_MEM_CHECK

#if defined(WIN32) || defined(WIN64)
#define new     new( __MRL_FILE__, __LINE__ )
#else
#define new(...)      new( __VA_ARGS__, __MRL_FILE__, __LINE__ )
#endif

#endif // MR_MEM_CHECK

#endif
