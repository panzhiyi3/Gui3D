/*
    Gui3DEx
    -------
    
    Copyright (c) 2012 Valentin Frechaud
                                                                                  
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
                                                                                  
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
                                                                                  
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#ifndef Gui3D_H
#define Gui3D_H

#include <map>
#include <vector>

#include <Ogre.h>

#include "Gorilla.h"
#include "Gui3DScreen.h"
#include "Gui3DPanelColors.h"
#include "Common.h"

namespace Gui3DEx
{
// TODO : Screens methods : implements something more ?

/*! class. Gui3D
    desc.
        The main object to instanciate Gui3D.
*/
class GUI3D_EXPORTS Gui3D
{
public:
    /** \brief The PanelColors gave as argument will automatically be deleted in Gui3D destructor, or
    in the setPanelColors() method
    */
    Gui3D(PanelColors *);
    Gui3D();

    virtual ~Gui3D();

    /** \brief /!\ Be careful when using this. Return the main Gorilla object.
    */
    Gorilla::Silverback* getSilverback();

    void setDefaultFont(const std::string &fontName,
        const std::string &fontFilename,
        const int fontSize = 20,
        const int textureSize = 1024,
        const int fontResolution = 72,
        const bool antiAliased = false,
        const Ogre::String &resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    // ScreenRenderables methods (2D Screens rendered in 3D)

    Gorilla::Layer* getLayer(const Ogre::String &screenRenderableName,
                             const Ogre::String &layerName);

    Gorilla::Layer* getLayer(Gui3DScreenRenderable *screenRenderable,
                             const Ogre::String &layerName);

    Gorilla::Layer* createLayer(Gui3DScreenRenderable *screenRenderable,
                                const Ogre::String &layerName);

    Gorilla::Layer* createLayer(const Ogre::String &screenRenderableName,
                                const Ogre::String &layerName);

    Gui3DScreenRenderable *getScreenRenderable(const Ogre::String &name);

    Gui3DScreenRenderable *createScreenRenderable(const Ogre::Vector2 &pos, 
                                                      const Ogre::String &atlasName, 
                                                      const Ogre::String &name);

    // Screens methods (2D screen)

    Gorilla::Layer* getLayerScreen(const Ogre::String &screenName,
                                   const Ogre::String &layerName);

    Gorilla::Layer* getLayerScreen(Gui3DScreen *screen,
                                   const Ogre::String &layerName);

    Gorilla::Layer* createLayerScreen(Gui3DScreen *screen,
                                      const Ogre::String &layerName);

    Gorilla::Layer* createLayerScreen(const Ogre::String &screenName,
                                      const Ogre::String &layerName);

    Gui3DScreen *createScreen(Ogre::Viewport *vp, 
                                  const Ogre::String &atlasName, const Ogre::String &name);

    Gui3DScreen *getScreen(const Ogre::String &name);

    // General methods

    void loadAtlas(const Ogre::String &name);

    void destroyScreenRenderable(const Ogre::String &screenRenderableName);

    void destroyScreenRenderable(Gui3DScreenRenderable *screenRenderable);

    void destroyScreen(const Ogre::String &screenName);

    void destroyScreen(Gui3DScreen *screen);

    /** \brief Set the colors rules of the Gui3DEx. 
    The PanelColors object will automatically be destroyed by the Gui3DEx destructor.
    */
    void setPanelColors(PanelColors *);

    PanelColors* getPanelColors();

protected:
    Gorilla::Silverback* mSilverback;

    std::vector<Ogre::String> mAtlas;

    std::map<Ogre::String, Gui3DScreenRenderable *> mScreenRenderables;

    std::map<Ogre::String, Gui3DScreen *> mScreens;

    PanelColors* mPanelColors;

    bool hasAtlas(const Ogre::String& name);
};

}

#endif
