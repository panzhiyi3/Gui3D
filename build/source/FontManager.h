#pragma once

#include <map>
#include <vector>
#include "Ogre.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Gorilla
{
    class Font : public Ogre::GeneralAllocatedObject
    {
    public:
        typedef unsigned int CodePoint;
        typedef Ogre::FloatRect UVRect;
        // A range of code points, inclusive on both ends
        typedef std::pair<CodePoint, CodePoint> CodePointRange;
        typedef std::vector<CodePointRange> CodePointRangeList;

        class GlyphInfo: public Ogre::GeneralAllocatedObject
        {
        public:
            CodePoint codePoint;  //字符的unicode码
            UVRect uvRect;        //纹理区域
            float aspectRatio;
            unsigned int useCount;      //字符的使用次数
            unsigned int u1Pixel;       //u1像素坐标
            unsigned int v1Pixel;       //v1像素坐标
            unsigned int advanceX;
            unsigned int advanceY;

            GlyphInfo(CodePoint _code, const UVRect& _rect, float _aspect, unsigned int _u, unsigned int _v, unsigned int _advanceX, unsigned int _advanceY) 
                : codePoint(_code), uvRect(_rect), aspectRatio(_aspect), useCount(0), u1Pixel(_u), v1Pixel(_v), advanceX(_advanceX), advanceY(_advanceY)
            {
            }
        };

        Font(int textureSize = 1024, int ttfSize = 20.0, int ttfResolution = 72, bool antiAliased = false);
        virtual ~Font();

        virtual void load(const std::string &name, const std::string &fontName) = 0;

        virtual void unLoad();

        virtual const Ogre::String &getTextureName();

        virtual void addCodePointRange(const CodePointRange& range) = 0;

        /*
         * Clear the list of code point ranges.
         */
        virtual void clearCodePointRanges() = 0;

        virtual bool isCodeIdInRange(CodePoint id) const;

        /*
         * Get a const reference to the list of code point ranges to be used to
           generate glyphs from a truetype font.
         */
        virtual inline const CodePointRangeList& getCodePointRangeList() const = 0;

        virtual inline const UVRect& getGlyphTexCoords(CodePoint id) const = 0;

        /** Sets the texture coordinates of a glyph.
         @remarks
             You only need to call this if you're setting up a font loaded from a texture manually.
         @note
             Also sets the aspect ratio (width / height) of this character. textureAspect
             is the width/height of the texture (may be non-square)
        */
        //virtual inline void setGlyphTexCoords(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel, unsigned int u2Pixel, unsigned int v2Pixel, float textureAspect) = 0;

        virtual inline float getGlyphAspectRatio(CodePoint id) const = 0;

        virtual inline void setGlyphAspectRatio(CodePoint id, float ratio) = 0;

        /** Sets the texture coordinates of a glyph.
         @remarks
             You only need to call this if you're setting up a font loaded from a texture manually.
         @note
             Also sets the aspect ratio (width / height) of this character. textureAspect
             is the width/height of the texture (may be non-square)
        */
        virtual inline void setGlyphInfo(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel,
            unsigned int u2Pixel, unsigned int v2Pixel,
            unsigned int advanceX, unsigned int advanceY, float textureAspect) = 0;

        virtual const GlyphInfo *getGlyphInfo(CodePoint id) = 0;

        virtual void insertGlyphInfo(CodePoint id) = 0;

        virtual bool hasBlankInTexture() const = 0;

        virtual void renderGlyphIntoTexture(CodePoint id) = 0;

        virtual CodePoint getLessUseChar() = 0;

        virtual void removeGlyph(CodePoint id) = 0;

        virtual inline int getPointSize() const;

        virtual inline float getPixelSize() const;

    protected:
        // Map from unicode code point to texture coordinates
        typedef std::map<CodePoint, GlyphInfo> CodePointMap;
        CodePointMap mCodePointMap;

        // Range of code points to generate glyphs for (truetype only)
        CodePointRangeList mCodePointRangeList;

        std::string mFontPath;
        FT_Library mFtLibrary;
        FT_Face mFtFace;

        // Size of the truetype font, in points
        int mTtfSize;
        // Resolution (dpi) of truetype font
        unsigned int mTtfResolution;
        // Max distance to baseline of this (truetype) font
        int mTtfMaxBearingY;
        // for TRUE_TYPE font only
        bool mAntialiasColour;

        unsigned int mPixelBytes;
        unsigned int mCharDataWidth;
        unsigned int mMaxCharSize;
        unsigned int mDataSize;

        int mMaxHeight;
        int mMaxWidth;
        float mTextureAspect;
        unsigned int mCharSpacer;

        Ogre::String mTextureName;
        // 纹理上使用区域还剩的个数
        unsigned int mLeftBlankNum;
        unsigned int mMaxCharNum;
        unsigned char *mImageData;

        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mImage_v;
        unsigned int mImage_u;
    };

    class IFontCallback
    {
    public:
        virtual void onFontTextureDirty(const Ogre::String &texName) = 0;
    };

    class FontManager
    {
    public:
        FontManager();
        virtual ~FontManager();

        virtual void setCallback(IFontCallback *callback);

        virtual void setDefaultFont(const Ogre::String &fontName,
            const Ogre::String &fontFilename,
            const int fontSize = 20,
            const int textureSize = 1024,
            const int fontResolution = 72,
            const bool antiAliased = false,
            const Ogre::String &resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        virtual Font *getDefaultFont() { return mDefaultFont; }

        virtual const Font::GlyphInfo *getGlyphInfo(Font::CodePoint id);

    private:
        Font *mDefaultFont;
        IFontCallback *mCallback;
    };
}
