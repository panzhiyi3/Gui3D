#include "Ogre.h"
#include "FreeTypeFont.h"
#include "FontManager.h"
#include "OgreTextureManager.h"

using namespace Ogre;

namespace Gorilla
{
    const Ogre::String FONT_TEXTURE_BASENAME = "GUI3D_FreeTypeFontTexture";

    FreeTypeFont::FreeTypeFont(int textureSize, int ttfSize, int ttfResolution, bool antiAliased)
        : Font(textureSize, ttfSize, ttfResolution, antiAliased)
    {
        mTextureName = FONT_TEXTURE_BASENAME;
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

        //初始化库 
        if(FT_Init_FreeType(&ftLibrary))
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "FreeType init fialed", "");
        }

        mCharSpacer = 5;
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
            for(Font::CodePoint cp = range.first; cp <= range.second; ++cp)
            {
                FT_Load_Char( mFtFace, cp, FT_LOAD_RENDER);

                if( ( 2 * ( mFtFace->glyph->bitmap.rows << 6 ) - mFtFace->glyph->metrics.horiBearingY ) > mMaxHeight )
                    mMaxHeight = ( 2 * ( mFtFace->glyph->bitmap.rows << 6 ) - mFtFace->glyph->metrics.horiBearingY );
                if( mFtFace->glyph->metrics.horiBearingY > mTtfMaxBearingY )
                    mTtfMaxBearingY = mFtFace->glyph->metrics.horiBearingY;

                if( (mFtFace->glyph->advance.x >> 6 ) + ( mFtFace->glyph->metrics.horiBearingX >> 6 ) > mMaxWidth)
                    mMaxWidth = (mFtFace->glyph->advance.x >> 6 ) + ( mFtFace->glyph->metrics.horiBearingX >> 6 );
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
        LogManager::getSingleton().logMessage("Font texture size " + Ogre::StringConverter::toString(mWidth) + " * " + Ogre::StringConverter::toString(mHeight));

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

    void FreeTypeFont::addCodePointRange(const FreeTypeFont::CodePointRange &range)
    {
        mCodePointRangeList.push_back(range);
    }

    void FreeTypeFont::clearCodePointRanges()
    {
        mCodePointRangeList.clear();
    }

    const Font::CodePointRangeList &FreeTypeFont::getCodePointRangeList() const
    {
        return mCodePointRangeList;
    }

    const Font::UVRect &FreeTypeFont::getGlyphTexCoords(Font::CodePoint id) const
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

    void FreeTypeFont::setGlyphInfo(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel, unsigned int u2Pixel, unsigned int v2Pixel, unsigned int advance, float textureAspect)
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
            i->second.advance = advance;
        }
        else
        {
            mCodePointMap.insert(
                Font::CodePointMap::value_type(id, 
                Font::GlyphInfo(id, Font::UVRect(u1, v1, u2, v2), 
                textureAspect * (u2 - u1)  / (v2 - v1), u1Pixel, v1Pixel, advance)));
        }
    }

    const FreeTypeFont::GlyphInfo *FreeTypeFont::getGlyphInfo(CodePoint id) const
    {
        CodePointMap::const_iterator i = mCodePointMap.find(id);
        if (i == mCodePointMap.end())
        {
            return NULL;
        }
        return &i->second;
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
        unsigned char* buffer = mFtFace->glyph->bitmap.buffer;

        if (!buffer)
        {
            // Yuck, FT didn't detect this but generated a null pointer!
            LogManager::getSingleton().logMessage("Info: Freetype returned null for character " + StringConverter::toString(id));
        }

        int y_bearnig = ( mTtfMaxBearingY >> 6 ) - ( mFtFace->glyph->metrics.horiBearingY >> 6 );
        int x_bearing = mFtFace->glyph->metrics.horiBearingX >> 6;

        for(int j = 0; j < mFtFace->glyph->bitmap.rows; ++j )
        {
            size_t row = j + mImage_v + y_bearnig;
            UCHAR *pDest = &mImageData[(row * mWidth * mPixelBytes) + (mImage_u + x_bearing) * mPixelBytes];
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
            mImage_u + ( mFtFace->glyph->advance.x >> 6 ), // u2
            mImage_v + ( mMaxHeight >> 6 ), // v2
            //advance,
            20,
            mTextureAspect
            );

        // Advance a column
        mImage_u += (advance + mCharSpacer);

        // If at end of row
        if( mWidth - 1 < mImage_u + ( advance ) )
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

        //Image img;
        //tex->convertToImage(img);
        //img.save("H:\\test.png");

/*
        D3DLOCKED_RECT lockedRect;
        mTexture->LockRect(0, &lockedRect,0, 0);

        //使用类型注意
        uchar* TexData = (uchar*)lockedRect.pBits;

        for(UINT i = 0; i < mHeight; ++i)
        {
            for(UINT j = 0; j < mWidth; ++j)
            {
                //Pitch数据的总长度
                int index = i * lockedRect.Pitch / mPixelBytes + j;
                TexData[index] = mImageData[index];
            }
        }
        mTexture->UnlockRect(0);
*/

        // for test
        //#ifdef    _DEBUG
        //        D3DXSaveTextureToFileA("..//media//test.png",D3DXIFF_PNG, mTexture, 0);
        //#endif
    }

    void FreeTypeFont::insertGlyphInfo(CodePoint id)
    {
        if(!hasBlankInTexture()) // has no space left in texture    
        {
            removeGlyph(getLessUseChar());
        }
        renderGlyphIntoTexture(id);
    }

    bool FreeTypeFont::hasBlankInTexture() const 
    { 
        return mLeftBlankNum > 0; 
    }

    Font::CodePoint FreeTypeFont::getLessUseChar()
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
