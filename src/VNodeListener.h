/*
 * VNodeListener.h
 *
 *  Created on: Feb 17, 2011
 *      Author: balazsbela
 */

#ifndef VNODELISTENER_H_
#define VNODELISTENER_H_

#include <OgreSceneManager.h>
#include <OgreNode.h>
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"

class VisualizerApplication;
using namespace Ogre;
class VNodeListener : public Node :: Listener {
public:
	VNodeListener();
	virtual ~VNodeListener();
	void nodeUpdated(const Node *);
	bool wasUpdated();
	void clearUpdated();
	void setParent(VisualizerApplication* p);
	//Node* getNode();
private:
	//Node* m_Node;
	bool updated;
	VisualizerApplication* parent;
};

#endif /* VNODELISTENER_H_ */
