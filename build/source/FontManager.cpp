#include "Ogre.h"
#include "OgreTextureManager.h"
#include "FontManager.h"
#include "FreeTypeFont.h"

using namespace Gorilla;

Font::Font(int textureSize, int ttfSize, int ttfResolution, bool antiAliased)
:mTtfMaxBearingY(0)
,mTtfResolution(0)
,mAntialiasColour(antiAliased)
,mLeftBlankNum(0)
,mImageData(NULL)
,mImage_v(0)
,mImage_u(0)
{
    mWidth = textureSize;
    mHeight = textureSize;
    mTtfSize = ttfSize;
    mTtfResolution = ttfResolution;

    mTextureName = "FONT_TEXTURE_BASENAME";
}

Font::~Font()
{
    unLoad();
}

void Font::unLoad()
{
}

const Ogre::String &Font::getTextureName()
{
    return mTextureName;
}

bool Font::isCodeIdInRange(Font::CodePoint id) const
{
    for (Font::CodePointRangeList::const_iterator r = mCodePointRangeList.begin();
        r != mCodePointRangeList.end(); ++r)
    {
        const Font::CodePointRange &range = *r;
        if(id >= range.first && id <= range.second)
            return true;
    }
    return false;
}

int Font::getPointSize() const
{
    return mTtfSize;
}

float Font::getPixelSize() const
{
    float value = (float) mTtfSize / 72 * mTtfResolution;
    return value;
}

FontManager::FontManager()
: mDefaultFont(NULL)
, mCallback(NULL)
{
}

FontManager::~FontManager()
{
    if(mDefaultFont)
        delete mDefaultFont;
}

void FontManager::setCallback(IFontCallback *callback)
{
    mCallback = callback;
}

void FontManager::setDefaultFont(const Ogre::String &fontName,
                                 const Ogre::String &fontFilename,
                                 const int fontSize,
                                 const int textureSize,
                                 const int fontResolution,
                                 const bool antiAliased,
                                 const Ogre::String &resourceGroup)
{
    if(mDefaultFont)
    {
        delete mDefaultFont;
    }

    mDefaultFont = new FreeTypeFont(textureSize, fontSize, fontResolution, antiAliased);
    mDefaultFont->load(fontFilename, fontName);
}

const Font::GlyphInfo *FontManager::getGlyphInfo(Font::CodePoint id)
{
    if(mDefaultFont)
    {
        const Font::GlyphInfo *info = mDefaultFont->getGlyphInfo(id);
        if(info)
        {
            return info;
        }
        else
        {
            mDefaultFont->insertGlyphInfo(id);
            info = mDefaultFont->getGlyphInfo(id);
            if(info && mCallback)
            {
                mCallback->onFontTextureDirty(mDefaultFont->getTextureName());
            }
            return info;
        }
    }

    return NULL;
}
