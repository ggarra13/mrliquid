
#ifndef factoryBehavior_h
#define factoryBehavior_h

#include <maya/MPxDragAndDropBehavior.h>

class factoryBehavior : public MPxDragAndDropBehavior
{
     virtual bool    shouldBeUsedFor( MObject& sourceNode, 
				      MObject& destinationNode, 
				      MPlug& sourcePlug, 
				      MPlug& destinationPlug);

     virtual MStatus  connectNodeToNode( MObject &sourceNode, 
					 MObject &destinationNode, 
					 bool force );
};

#endif // factoryBehavior_h
