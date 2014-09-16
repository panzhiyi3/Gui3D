#include "Gui3DScreen.h"

namespace Gui3DEx
{
    Gorilla::Layer *Gui3DScreen::createLayer(Ogre::uint index)
    {
        if(mScreen)
        {
            return mScreen->createLayer(index);
        }
        return NULLPTR;
    }

    void Gui3DScreen::destroy(Gorilla::Layer *layer)
    {
        if(mScreen)
        {
            mScreen->destroy(layer);
        }
    }

    Gorilla::Layer *Gui3DScreenRenderable::createLayer(Ogre::uint index)
    {
        if(mScreenRenderable)
        {
            return mScreenRenderable->createLayer(index);
        }
        return NULLPTR;
    }
}