/*
 * CubeGenerator.h
 *
 *  Created on: Feb 13, 2011
 *      Author: balazsbela
 */

#ifndef CUBEGENERATOR_H_
#define CUBEGENERATOR_H_

#include <OgreColourValue.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreSceneManager.h>
#include <OgreMeshManager.h>
#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#include <OgreSubMesh.h>

using namespace Ogre;

class CubeGenerator {
public:
	static CubeGenerator* getInstance();
	virtual ~CubeGenerator();
	void createColourCube(ColourValue cv,String name);
	SceneNode* generateCube(SceneManager* sceneManager,ColourValue color,Vector3 position);
private:
	CubeGenerator();
	static CubeGenerator* instance;
	int m_cubeIndex;

};

#endif /* CUBEGENERATOR_H_ */
