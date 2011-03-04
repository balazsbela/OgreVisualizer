#include "VisualizerApplication.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_audio.h"
#include <iostream>

using namespace Ogre;

VisualizerApplication::VisualizerApplication(void)
{
	m_sem = SDL_CreateSemaphore(1);
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * 512);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * 512);

}

VisualizerApplication::~VisualizerApplication(void)
{
	Mix_UnregisterAllEffects(0);
	Mix_HaltMusic();
	Mix_CloseAudio();

	SDL_DestroySemaphore(m_sem);

	windowClosed(mWindow);
	fftw_free(in);
	fftw_free(out);
}
 
void musicData (int chan, void *stream, int len, void *udata)
{

	if(udata) {
		VisualizerApplication *vapp = (VisualizerApplication*) udata;
		vapp -> lock();
		vapp -> visualize(chan,(int*) stream,len);
		vapp -> unlock();
	}

}


void musicDone(int chan, void *stream)
{
	std::cout << "Music stopped playing!";
}

//-------------------------------------------------------------------------------------
void VisualizerApplication::createScene(void)
{
 	mCamera->setPosition(Ogre::Vector3(190,272,-465));
	mCamera->lookAt(Ogre::Vector3(0.015,0.98,0.16));

	int x = 0;
	int z = 0;
	float r_base;
	float b_base;


	for(int i=0;i<16;i++) {
		b_base = i * (1.0/15);
		r_base = 1.0 - b_base;

		for(int j=0;j<16;j++) {
			m_nodes[i][j] = CubeGenerator::getInstance() ->generateCube(mSceneMgr,ColourValue(r_base-(j*(r_base/15)),j*(1.0/15),b_base),Vector3(x, 0, z));
			x+=45;
		}
		z += 45;
		x = 0;
	}

	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	// Create a light
	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);

	playMusic();
}



int VisualizerApplication :: playMusic(void) {
	if(SDL_Init(SDL_INIT_AUDIO) < 0 ) {
		 Ogre::LogManager::getSingletonPtr()->logMessage(SDL_GetError());
	}

	if( Mix_OpenAudio(44100, AUDIO_S16, 2, 512) < 0 ) {
		 Ogre::LogManager::getSingletonPtr()->logMessage(Mix_GetError());
	}

	Mix_Music* music = Mix_LoadMUS("music.ogg");
	if(music==NULL) {
		Ogre::LogManager::getSingletonPtr()->logMessage(Mix_GetError());
	}

	if(Mix_PlayMusic(music,0) < 0) {
		Ogre::LogManager::getSingletonPtr()->logMessage(Mix_GetError());
	}

	if(Mix_RegisterEffect(MIX_CHANNEL_POST,musicData,musicDone,this) < 0) {
		//Could not register effect
		Ogre::LogManager::getSingletonPtr()->logMessage(Mix_GetError());
	}

	return 0;
}

//Our callback function with all stream data
void VisualizerApplication :: visualize(int chan,int* stream,int len) {
	 fftw_plan p;
	 int N = 512;

	 if(len < N) {
		 return;
	 }

	 int cut = len / 512;

	 int j = 0;
	 for(int i=0;i<len;i+=cut) {
		 if( (cut%2 == 0) || (i==0) ) {
			 //std::cout << "Stream data:" << stream[i] << " " << stream[i+1]<< " ";
			 in[j][0] = (stream[i]/2 + stream[i+1]/2)/256;
			 in[j][1] = 0;
		 }
		 else {
			// y = (stream[i-1]*(1.0/65536)) * (stream[i]*(1.0/65536));
			//std::cout << "Stream data:" << stream[i-1] << " " << stream[i] << " ";
			in[j][0] = (stream[i-1]/2 + stream[i]/2)/256;
			in[j][1] = 0;
		 }

		// std::cout << j<< " " <<in[j][0] << "\n";
		 j++;
		 if (j==512) {
			 break;
		 }
	 }

	 p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	 fftw_execute(p);

	/*std::cout << "\n\n";
	std::cout << len << " " << cut << " " << j <<"\n";
	long double result;
	for(int i=0;i<N;i++) {

		//std::cout <<in[i][0] << "==>" << out[i][0] << ":" << out[i][1] << "\n";

		result = sqrt( pow((out[i][0]),2) + pow((out[i][1]),2) ) * (1.0/65536);
		//std::cout << out[i][0] <<"+"<<out[i][1] << "*i ;";
		std::cout << i << ":" << abs(in[i][0])*(1.0/65536) << ":" <<  result  << "\n";
	}

	std::cout << "\n" << "\n";
	*/

	for ( int i = 0 ; i < 256 ; i++ ) {
				m_spec[i] = sqrt( pow((out[i][0]),2) + pow((out[i][1]),2) );
			//	std::cout << i <<":" << m_spec[i] << "\n";
	}
	fftw_destroy_plan(p);

	renderFreq();

}

void VisualizerApplication :: renderFreq() {
	int y,i,c;
	long double val;
	double scale = 1.0 / log(256.0);
	int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

	for(y = 15; y > 0; y--)
			for(i = 0; i < 16; i++)
				m_heights[y][i] = m_heights[y - 1][i];


	for(i = 0; i < 16 ; i++) {
			for(c = xscale[i], y = 0; c < xscale[i + 1]; c++) {
				if(m_spec[c] > y)
					y = m_spec[c];
			}
			y >>= 7;
			if(y > 0)
				val = (log(y) * scale);
			else
				val = 0;

			m_heights[0][i] = val;
		}

	adjustNodes();
}

void VisualizerApplication :: adjustNodes() {
	Vector3 pos;
	float temp;
		for(int i=0;i<16;i++) {
			for(int j=0;j<16;j++) {
				if(m_nodes[i][j]) {
					temp = m_heights[i][j]/2.3f;
					if(temp > 0.5) {
						temp = m_heights[i][j]/2.3f - 0.5;
					}
					if(temp < 0.2) {
						temp = 0;
					}
					pos = m_nodes[i][j] -> getPosition();
				/*	m_vnodeListener.clearUpdated();
					m_nodes[i][j] -> setListener(&m_vnodeListener);*/


					m_nodes[i][j] -> setScale(Vector3(0.1,temp,0.1));
					m_nodes[i][j] -> setPosition(pos.x,temp*100/2.0f,pos.z);
					//m_nodes[i][j] -> setScale(Vector3(0.1,0,0.1));
					//wait while node is updated;
					//m_heights[i][j] = 0;
				}
				else {
					std::cout << m_nodes[i][j] << " at " << i << " " << "j";
				}
			}
		}
}


void VisualizerApplication :: lock() {
	SDL_SemWait(m_sem);
}


void VisualizerApplication :: unlock() {
	SDL_SemPost(m_sem);
}



