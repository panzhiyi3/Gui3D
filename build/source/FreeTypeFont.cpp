#include "Ogre.h"
#include "FreeTypeFont.h"
#include "FontManager.h"
#include "OgreTextureManager.h"

using namespace Ogre;

#define BEARING_X_SPACER 1 //·ÀÖ¹×Ö·ûÌùÍ¼×ó¶Ë¶¥¸ñ£¬µ¼ÖÂÏÔÊ¾¾â³Ý

typedef unsigned char UCHAR;
typedef unsigned int UINT;

namespace Gorilla
{
    const Ogre::String FONT_TEXTURE_BASENAME = "GUI3D_FreeTypeFontTexture";

    FreeTypeFont::FreeTypeFont(int textureSize, int ttfSize, int ttfResolution, bool antiAliased)
        : Font(textureSize, ttfSize, ttfResolution, antiAliased)
    {
        mTextureName = FONT_TEXTURE_BASENAME;
        mCharSpacer = 2;
    }

    FreeTypeFont::~FreeTypeFont()
    {
        //Font class will call unLoad()
    }

    void FreeTypeFont::load(const std::string &name, const std::string &fontName)
    {
        mFontPath = name;
        mTextureName += fontName;

        FT_Library ftLibrary;

        //³õÊ¼»¯¿â 
        if(FT_Init_FreeType(&ftLibrary))
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "FreeType init fialed", "");
        }

        if(FT_New_Face(ftLibrary, name.c_str(), 0, &mFtFace)) 
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "FreeType can't open TTF file", "");
        }

        UINT maxFaceNum = mFtFace->num_faces;

        FT_F26Dot6 ftSize = (FT_F26Dot6)(mTtfSize * (1 << 6));

        if(FT_Set_Char_Size( mFtFace, ftSize, 0, mTtfResolution, mTtfResolution))
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Could not set char size!", "");
        }

        mMaxHeight = 0;
        mMaxWidth = 0;

        if(mCodePointRangeList.empty())
        {
            mCodePointRangeList.push_back(CodePointRange(33, 166));
            mCodePointRangeList.push_back(CodePointRange(19968, 40869));
        }

        // Calculate maximum width, height and bearing
        for (Font::CodePointRangeList::const_iterator r = mCodePointRangeList.begin();
            r != mCodePointRangeList.end(); ++r)
        {
            const Font::CodePointRange& range = *r;
            int horiBearingX = 0;
            int totalWidth = 0;
            for(Font::CodePoint cp = range.first; cp <= range.second; ++cp)
            {
                FT_Load_Char( mFtFace, cp, FT_LOAD_RENDER);

                if( ( 2 * ( mFtFace->glyph->bitmap.rows << 6 ) - mFtFace->glyph->metrics.horiBearingY ) > mMaxHeight )
                    mMaxHeight = ( 2 * ( mFtFace->glyph->bitmap.rows << 6 ) - mFtFace->glyph->metrics.horiBearingY );
                if( mFtFace->glyph->metrics.horiBearingY > mTtfMaxBearingY )
                    mTtfMaxBearingY = mFtFace->glyph->metrics.horiBearingY;

                horiBearingX = mFtFace->glyph->metrics.horiBearingX >> 6;
                if(horiBearingX == 0)
                {
                    horiBearingX = BEARING_X_SPACER;
                }
                totalWidth = (mFtFace->glyph->advance.x >> 6 ) + horiBearingX;
                if(totalWidth <= mFtFace->glyph->bitmap.width + 1)
                {
                    totalWidth = mFtFace->glyph->bitmap.width + BEARING_X_SPACER;
                }
                if( totalWidth > mMaxWidth)
                    mMaxWidth = totalWidth;
            }
        }

        // We just make a 1024 * 1024 texture, it's enough
        mTextureAspect = 1.0f;

        mPixelBytes = 4;
        mCharDataWidth = (mMaxWidth + mCharSpacer) * mPixelBytes;
        mDataSize= mWidth * mHeight * mPixelBytes;
        mMaxCharSize = ((mMaxHeight >> 6) + mCharSpacer) * mCharDataWidth;
        mMaxCharNum = mDataSize / mMaxCharSize;
        mLeftBlankNum = mMaxCharNum;
        LogManager::getSingleton().logMessage("Font texture size: " + Ogre::StringConverter::toString(mWidth) + " * " + Ogre::StringConverter::toString(mHeight)
            + "with left blank: " + Ogre::StringConverter::toString(mLeftBlankNum));

        mImageData = new unsigned char[mDataSize];
        // Reset content (transparent, white)
        for (size_t i = 0; i < mDataSize; i += mPixelBytes)
        {
            mImageData[i + 0] = 0xFF; // luminance
            mImageData[i + 1] = 0xFF;
            mImageData[i + 2] = 0xFF;
            mImageData[i + 3] = 0x00; // alpha
        }
    }

    void FreeTypeFont::unLoad()
    {
        if(mImageData)
        {
            delete[] mImageData;
        }

        FT_Done_FreeType(mFtLibrary);
    }

    //void FreeTypeFont::setGlyphTexCoords(Font::CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel, unsigned int u2Pixel, unsigned int v2Pixel, float textureAspect)
    //{
    //    float u1 = (float)u1Pixel / (float)mWidth, v1 = (float)v1Pixel / (float)mHeight, u2 = (float)u2Pixel / (float)mWidth, v2 = (float)v2Pixel / (float)mWidth;
    //    Font::CodePointMap::iterator i = mCodePointMap.find(id);
    //    if (i != mCodePointMap.end())
    //    {
    //        i->second.uvRect.left = u1;
    //        i->second.uvRect.top = v1;
    //        i->second.uvRect.right = u2;
    //        i->second.uvRect.bottom = v2;
    //        i->second.aspectRatio = textureAspect * (u2 - u1)  / (v2 - v1);
    //        i->second.u = u1Pixel;
    //        i->second.v = v1Pixel;
    //    }
    //    else
    //    {
    //        mCodePointMap.insert(
    //            Font::CodePointMap::value_type(id, 
    //            Font::GlyphInfo(id, Font::UVRect(u1, v1, u2, v2), 
    //            textureAspect * (u2 - u1)  / (v2 - v1), u1Pixel, v1Pixel)));
    //    }
    //}

    inline float FreeTypeFont::getGlyphAspectRatio(CodePoint id) const
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

    inline void FreeTypeFont::setGlyphAspectRatio(CodePoint id, float ratio)
    {
        CodePointMap::iterator i = mCodePointMap.find(id);
        if (i != mCodePointMap.end())
        {
            i->second.aspectRatio = ratio;
        }
    }

    void FreeTypeFont::setGlyphInfo(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel,
        unsigned int u2Pixel, unsigned int v2Pixel,
        unsigned int advanceX, unsigned int advanceY, float textureAspect)
    {
        float u1 = (float)u1Pixel / (float)mWidth, v1 = (float)v1Pixel / (float)mHeight, u2 = (float)u2Pixel / (float)mWidth, v2 = (float)v2Pixel / (float)mWidth;
        Font::CodePointMap::iterator i = mCodePointMap.find(id);
        if (i != mCodePointMap.end())
        {
            i->second.uvRect.left = u1;
            i->second.uvRect.top = v1;
            i->second.uvRect.right = u2;
            i->second.uvRect.bottom = v2;
            i->second.aspectRatio = textureAspect * (u2 - u1)  / (v2 - v1);
            i->second.u1Pixel = u1Pixel;
            i->second.v1Pixel = v1Pixel;
            i->second.advanceX = advanceX;
            i->second.advanceY = advanceY;
        }
        else
        {
            mCodePointMap.insert(
                Font::CodePointMap::value_type(id, 
                Font::GlyphInfo(id, Font::UVRect(u1, v1, u2, v2), 
                textureAspect * (u2 - u1)  / (v2 - v1), u1Pixel, v1Pixel, advanceX, advanceY)));
        }
    }

    void FreeTypeFont::renderGlyphIntoTexture(CodePoint id)
    {
        FT_Error ftResult;

        // Load & render glyph
        ftResult = FT_Load_Char( mFtFace, id, FT_LOAD_RENDER );
        if (ftResult)
        {
            // problem loading this glyph, continue
            LogManager::getSingleton().logMessage("Info: cannot load CodePoint %d" + StringConverter::toString(id));
        }

        FT_Int advance = mFtFace->glyph->advance.x >> 6;
        //FT_Int advance = mFtFace->glyph->metrics.horiAdvance >> 6;
        unsigned char *buffer = mFtFace->glyph->bitmap.buffer;

        if (!buffer)
        {
            // Yuck, FT didn't detect this but generated a null pointer!
            LogManager::getSingleton().logMessage("Info: Freetype returned null for character " + StringConverter::toString(id));
        }

        int y_bearnig = ( mTtfMaxBearingY >> 6 ) - ( mFtFace->glyph->metrics.horiBearingY >> 6 );
        int x_bearing = mFtFace->glyph->metrics.horiBearingX >> 6;
        int x_bearing_spacer = 0;
        if(x_bearing == 0)
        {
            x_bearing_spacer = BEARING_X_SPACER;
        }
        int totalWidth = (mFtFace->glyph->advance.x >> 6 ) + x_bearing_spacer;
        int x_bearing_right = 0;
        if(totalWidth <= mFtFace->glyph->bitmap.width + 1)
        {
            x_bearing_right = BEARING_X_SPACER;
        }

        for(int j = 0; j < mFtFace->glyph->bitmap.rows; ++j )
        {
            size_t row = j + mImage_v + y_bearnig;
            UCHAR *pDest = &mImageData[(row * mWidth * mPixelBytes) + (mImage_u + x_bearing_spacer + x_bearing) * mPixelBytes];
            for(int k = 0; k < mFtFace->glyph->bitmap.width; ++k )
            {
                if (mAntialiasColour)
                {
                    // Use the same greyscale pixel for all components RGBA
                    *pDest++= *buffer;
                    *pDest++= *buffer;
                    *pDest++= *buffer;
                }
                else
                {
                    // Always white whether 'on' or 'off' pixel, since alpha
                    // will turn off
                    *pDest++= 0xFF;
                    *pDest++= 0xFF;
                    *pDest++= 0xFF;
                }
                // Always use the greyscale value for alpha
                *pDest++= *buffer++;
            }
        }

        this->setGlyphInfo(id,
            mImage_u,  // u1
            mImage_v,  // v1
            mImage_u + x_bearing_spacer + x_bearing_right + ( mFtFace->glyph->advance.x >> 6 ), // u2
            mImage_v + ( mMaxHeight >> 6 ), // v2
            mFtFace->glyph->advance.x >> 6, //mFtFace->glyph->metrics.horiAdvance >> 6,
            mMaxHeight >> 6, //mFtFace->glyph->metrics.vertAdvance >> 6,
            mTextureAspect
            );

        // Advance a column
        //mImage_u += (advance + mCharSpacer); //BUG
        mImage_u += mMaxWidth + mCharSpacer;

        // If at end of row
        if( mWidth - 1 < mImage_u + ( mMaxWidth ) )
        {
            mImage_v += ( mMaxHeight >> 6 ) + mCharSpacer;
            mImage_u = 0;
        }
        --mLeftBlankNum;

        DataStreamPtr dataSream(new Ogre::MemoryDataStream(mImageData, mWidth * mHeight * mPixelBytes));

        TextureManager::getSingleton().remove(mTextureName);
        TexturePtr tex = TextureManager::getSingleton().loadRawData(
            mTextureName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            dataSream,
            mWidth,
            mHeight,
            Ogre::PF_A8R8G8B8,
            Ogre::TEX_TYPE_2D,
            0);

        //static int g_count=0;
        //if(g_count==3)
        //{
        //    g_count=0;
        //    Image img;
        //    tex->convertToImage(img);
        //    img.save("H:\\test.png");
        //}
        //g_count++;
    }

    void FreeTypeFont::insertGlyphInfo(CodePoint id)
    {
        if(!hasBlankInTexture()) // has no space left in texture    
        {
            removeGlyph(getLessUseChar());
        }
        renderGlyphIntoTexture(id);
    }

    void FreeTypeFont::removeGlyph(CodePoint id)
    {
        CodePointMap::iterator it = mCodePointMap.find(id);
        if(it != mCodePointMap.end())
        {
            mImage_u = it->second.u1Pixel;
            mImage_v = it->second.v1Pixel;
            mCodePointMap.erase(it);
            ++mLeftBlankNum;
        }
        else
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Can not find CodePoint to remove", "Font::removeGlyph(CodePoint id)");
        }
    }
}
