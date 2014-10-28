#ifndef Gui3DScreen_H
#define Gui3DScreen_H

#include <map>
#include "Ogre.h"
#include "Gorilla.h"

namespace Gui3DEx
{
    class GUI3D_EXPORTS Gui3DScreen
    {
    public:
        Gui3DScreen(Gorilla::Screen* screen)
            : mScreen(screen)
        {}

        Gorilla::Layer *createLayer(Ogre::uint index = 0);
        void destroy(Gorilla::Layer *layer);

        Gorilla::Screen *mScreen;
        std::map<Ogre::String, Gorilla::Layer *> mLayers;
    };

    class Gui3DScreenRenderable
    {
    public:
        Gui3DScreenRenderable(Gorilla::ScreenRenderable* screenRenderable)
            : mScreenRenderable(screenRenderable)
        {}

        Gorilla::Layer *createLayer(Ogre::uint index = 0);

        Gorilla::ScreenRenderable *mScreenRenderable;
        std::map<Ogre::String, Gorilla::Layer *> mLayers;
    };
}

#endif
