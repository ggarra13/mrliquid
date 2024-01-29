//
// A simple convenient stack class.  Like the STL class, but only pointers
// stored.  Pointers are deleted once an element is popped().
//
// a = new mrInheritableFlags;
// a.update(fn);
// stack.push( a );

#ifndef mrStack_h
#define mrStack_h

#include <stack>

template< typename T >
class mrStack : public std::stack< T* >
{
     typedef std::stack< T* > super;

   public:
     inline void pop()
     {
	T* last = super::top();
	delete last;
	super::pop();
     }

     inline void clear()
     {
	while ( !super::empty() )
	{
	   pop();
	}
     }

     inline ~mrStack()
     {
	clear();
     }
};

#endif // mrStack_h
