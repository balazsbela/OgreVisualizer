/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include <OgreColourValue.h>
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "CubeGenerator.h"
#include <fftw3.h>
#include "VNodeListener.h"


class VisualizerApplication : public BaseApplication
{
public:
	VisualizerApplication();
	virtual ~VisualizerApplication(void);
    void visualize(int chan,int* stream,int len);
    int playMusic(void);
    void lock();
    void unlock();

protected:
    virtual void createScene(void);
private:

    void renderFreq();
    void adjustNodes();


    SDL_sem* m_sem;
    long int m_spec[256];
    float m_heights[16][16];
    SceneNode* m_nodes[16][16];
    fftw_complex *in, *out;
};

#endif // #ifndef __TutorialApplication_h_
