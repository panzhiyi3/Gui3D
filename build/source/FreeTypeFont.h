#pragma once

#include <map>
#include <vector>
#include <ft2build.h>
#include "Ogre.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace std;

namespace Gorilla
{
    class Font : public Ogre::GeneralAllocatedObject
    {
    public:
        typedef unsigned int CodePoint;
        typedef Ogre::FloatRect UVRect;

        const Ogre::String FONT_TEXTURE_BASENAME;

        /// Information about the position and size of a glyph in a texture
        struct GlyphInfo
        {
        public:
            CodePoint codePoint;  //字符的unicode码
            UVRect uvRect;        //纹理区域
            float aspectRatio;
            USHORT useCount;      //字符的使用次数
            UINT l;
            UINT m;

        public:
            GlyphInfo(CodePoint _code, const UVRect& _rect, float _aspect, UINT _l, UINT _m) 
                : codePoint(_code), uvRect(_rect), aspectRatio(_aspect), useCount(0), l(_l), m(_m)
            {
            }
        };
        /// A range of code points, inclusive on both ends
        typedef std::pair<CodePoint, CodePoint> CodePointRange;
        typedef std::vector<CodePointRange> CodePointRangeList;
    protected:
        /// Map from unicode code point to texture coordinates
        typedef std::map<CodePoint, GlyphInfo> CodePointMap;
        CodePointMap mCodePointMap;

        /// Range of code points to generate glyphs for (truetype only)
        CodePointRangeList mCodePointRangeList;
    public:

        Font(int textureSize = 1024, float ttfSize = 20.0, int ttfResolution = 96);

        ~Font();

        void load(const std::string &name, const std::string &fontName);

        void unLoad();

        void addCodePointRange(const CodePointRange& range)
        {
            mCodePointRangeList.push_back(range);
        }

        /** Clear the list of code point ranges.
        */
        void clearCodePointRanges()
        {
            mCodePointRangeList.clear();
        }
        /** Get a const reference to the list of code point ranges to be used to
            generate glyphs from a truetype font.
        */
        const CodePointRangeList& getCodePointRangeList() const
        {
            return mCodePointRangeList;
        }

    protected:
        /// Size of the truetype font, in points
        float mTtfSize;
        /// Resolution (dpi) of truetype font
        unsigned int mTtfResolution;
        /// Max distance to baseline of this (truetype) font
        int mTtfMaxBearingY;
        /// for TRUE_TYPE font only
        bool mAntialiasColour;

        Ogre::String mTextureName;

        UINT mWidth;
        UINT mHeight;
        std::string mFontPath;

        /// 纹理上使用区域还剩的个数
        UINT mLeftBlankNum;
        UINT mMaxCharNum;
        unsigned char *mImageData;
        FT_Library mFtLibrary;
        FT_Face mFtFace;

        UINT mPixelBytes;
        UINT mCharDataWidth;
        UINT mMaxCharSize;
        UINT mDataSize;

        int mMaxHeight;
        int mMaxWidth;
        float mTextureAspect;
        UINT mCharSpacer;

        UINT mImage_m;
        UINT mImage_l;

    public:

        UINT getTextureWidth() const { return mWidth;}

        UINT getTextureHeight() const { return mHeight;}


        inline const UVRect& getGlyphTexCoords(CodePoint id) const
        {
            CodePointMap::const_iterator i = mCodePointMap.find(id);
            if (i != mCodePointMap.end())
            {
                return i->second.uvRect;
            }
            else
            {
                static UVRect nullRect(0.0, 0.0, 0.0, 0.0);
                return nullRect;
            }
        }

        /** Sets the texture coordinates of a glyph.
        @remarks
            You only need to call this if you're setting up a font loaded from a texture manually.
        @note
            Also sets the aspect ratio (width / height) of this character. textureAspect
            is the width/height of the texture (may be non-square)
        */
        inline void setGlyphTexCoords(CodePoint id, UINT u1Pixel, UINT v1Pixel, UINT u2Pixel, UINT v2Pixel, float textureAspect)
        {
            float u1 = (float)u1Pixel / (float)mWidth, v1 = (float)v1Pixel / (float)mHeight, u2 = (float)u2Pixel / (float)mWidth, v2 = (float)v2Pixel / (float)mWidth;
            CodePointMap::iterator i = mCodePointMap.find(id);
            if (i != mCodePointMap.end())
            {
                i->second.uvRect.left = u1;
                i->second.uvRect.top = v1;
                i->second.uvRect.right = u2;
                i->second.uvRect.bottom = v2;
                i->second.aspectRatio = textureAspect * (u2 - u1)  / (v2 - v1);
                i->second.l = u1Pixel;
                i->second.m = v1Pixel;
            }
            else
            {
                mCodePointMap.insert(
                    CodePointMap::value_type(id, 
                        GlyphInfo(id, UVRect(u1, v1, u2, v2), 
                            textureAspect * (u2 - u1)  / (v2 - v1), u1Pixel, v1Pixel)));
            }

        }

        /** Gets the aspect ratio (width / height) of this character. */
        inline float getGlyphAspectRatio(CodePoint id) const
        {
            CodePointMap::const_iterator i = mCodePointMap.find(id);
            if (i != mCodePointMap.end())
            {
                return i->second.aspectRatio;
            }
            else
            {
                return 1.0;
            }
        }
        /** Sets the aspect ratio (width / height) of this character.
        @remarks
            You only need to call this if you're setting up a font loaded from a 
            texture manually.
        */
        inline void setGlyphAspectRatio(CodePoint id, float ratio)
        {
            CodePointMap::iterator i = mCodePointMap.find(id);
            if (i != mCodePointMap.end())
            {
                i->second.aspectRatio = ratio;
            }
        }

        /** Gets the information available for a glyph corresponding to a
            given code point, or throws an exception if it doesn't exist;
        */
        const GlyphInfo *getGlyphInfo(CodePoint id) const;

        void insertGlyphInfo(CodePoint id);

        bool hasBlankInTexture() const 
        { 
            return mLeftBlankNum > 0; 
        }

        void renderGlyphIntoTexture(CodePoint id);

        CodePoint getLessUseChar();

        void removeGlyph(CodePoint id);
    };
}
