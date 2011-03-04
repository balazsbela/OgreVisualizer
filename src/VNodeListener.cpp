/*
 * VNodeListener.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: balazsbela
 */

#include "VNodeListener.h"
#include "VisualizerApplication.h"
#include <iostream>

VNodeListener::VNodeListener() : Listener() {
	updated = 0;
}

VNodeListener::~VNodeListener() {
	// TODO Auto-generated destructor stub
}

void VNodeListener :: nodeUpdated(const Node *n) {
	updated = 1;
	//parent -> nodeUpdateCallback();
}

bool  VNodeListener :: wasUpdated() {
	return updated;
}

void  VNodeListener :: clearUpdated() {
	updated = 0;
}

/*
Node* VNodeListener :: getNode() {
	return m_Node;
}
*/

void VNodeListener :: setParent(VisualizerApplication* p) {
	parent = p;
}
