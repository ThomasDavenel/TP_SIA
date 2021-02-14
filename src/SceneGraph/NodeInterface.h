#ifndef _SceneGraph_NodeInterface_H
#define _SceneGraph_NodeInterface_H

namespace SceneGraph
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \class	NodeInterface
	///
	/// \brief	Super class of all nodes in the scene graph.
	///
	/// \author	F. Lamarche, Université de Rennes 1
	/// \date	04/04/2016
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class NodeInterface
	{
	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	virtual void NodeInterface::draw() = 0;
		///
		/// \brief	Draws this node.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	04/04/2016
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void draw() = 0 ;
	};
}

#endif
