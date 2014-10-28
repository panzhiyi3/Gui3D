#include "Ogre.h"
#include "OgreTextureManager.h"
#include "FontManager.h"
#include "FreeTypeFont.h"

using namespace Gorilla;

Font::Font(int textureSize, int ttfSize, int ttfResolution, bool antiAliased)
: mTtfMaxBearingY(0)
, mTtfResolution(0)
, mAntialiasColour(antiAliased)
, mLeftBlankNum(0)
, mCharSpacer(0)
, mImageData(NULL)
, mImage_v(0)
, mImage_u(0)
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

void Font::addCodePointRange(const FreeTypeFont::CodePointRange &range)
{
    mCodePointRangeList.push_back(range);
}

void Font::clearCodePointRanges()
{
    mCodePointRangeList.clear();
}

const Font::CodePointRangeList &Font::getCodePointRangeList() const
{
    return mCodePointRangeList;
}

const Font::UVRect &Font::getGlyphTexCoords(Font::CodePoint id) const
{
    Font::CodePointMap::const_iterator i = mCodePointMap.find(id);
    if (i != mCodePointMap.end())
    {
        return i->second.uvRect;
    }
    else
    {
        static Font::UVRect nullRect(0.0, 0.0, 0.0, 0.0);
        return nullRect;
    }
}

const FreeTypeFont::GlyphInfo *Font::getGlyphInfo(CodePoint id)
{
    CodePointMap::iterator i = mCodePointMap.find(id);
    if (i == mCodePointMap.end())
    {
        return NULL;
    }
    if(i->second.useCount < 0xffffffff)
    {
        i->second.useCount++;
    }
    return &i->second;
}

bool Font::hasBlankInTexture() const 
{ 
    return mLeftBlankNum > 0; 
}

Font::CodePoint Font::getLessUseChar()
{
    CodePointMap::iterator itr = mCodePointMap.begin();
    CodePointMap::iterator itrEnd = mCodePointMap.end();
    CodePointMap::iterator itrLess = mCodePointMap.begin();
    while(itr != itrEnd)
    {
        if(itr->second.useCount < itrLess->second.useCount)
            itrLess = itr;
        itr++;
    }
    return itrLess->second.codePoint;   
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

int Font::getFontPointSize() const
{
    return mTtfSize;
}

float Font::getFontPixelSize() const
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

void FontManager::setDefaultFont(const std::string &fontName,
                                 const std::string &fontFilename,
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
