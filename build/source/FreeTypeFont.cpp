#include "Ogre.h"
#include "FreeTypeFont.h"
#include "OgreTextureManager.h"

using namespace Ogre;

namespace Gorilla
{
    const Ogre::String FONT_TEXTURE_BASENAME = "GUI3D_FreeTypeFontTexture";

    Font::Font(int textureSize, float ttfSize, int ttfResolution)
        :mTtfMaxBearingY(0)
        ,mTtfResolution(0)
        ,mAntialiasColour(false)
        ,mLeftBlankNum(0)
        ,mImageData(NULL)
        ,mImage_m(0)
        ,mImage_l(0)
    {
        mWidth = textureSize;
        mHeight = textureSize;
        mTtfSize = ttfSize;
        mTtfResolution = ttfResolution;

        mTextureName = FONT_TEXTURE_BASENAME;
    }

    Font::~Font()
    {
        unLoad();
    }

    void Font::load(const std::string &name, const std::string &fontName)
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
        for (CodePointRangeList::const_iterator r = mCodePointRangeList.begin();
            r != mCodePointRangeList.end(); ++r)
        {
            const CodePointRange& range = *r;
            for(CodePoint cp = range.first; cp <= range.second; ++cp)
            {
                FT_Load_Char( mFtFace, cp, FT_LOAD_RENDER );

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

    void Font::unLoad()
    {
        if(mImageData)
        {
            delete[] mImageData;
        }

        FT_Done_FreeType(mFtLibrary);
    }

    const Font::GlyphInfo *Font::getGlyphInfo(CodePoint id) const
    {
        CodePointMap::const_iterator i = mCodePointMap.find(id);
        if (i == mCodePointMap.end())
        {
            return NULL;
        }
        return &i->second;
    }

    void Font::renderGlyphIntoTexture(CodePoint id)
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
            size_t row = j + mImage_m + y_bearnig;
            UCHAR *pDest = &mImageData[(row * mWidth * mPixelBytes) + (mImage_l + x_bearing) * mPixelBytes];
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

        this->setGlyphTexCoords(id,
            mImage_l,  // u1
            mImage_m,  // v1
            mImage_l + ( mFtFace->glyph->advance.x >> 6 ), // u2
            mImage_m + ( mMaxHeight >> 6 ), // v2
            mTextureAspect
            );

        // Advance a column
        mImage_l += (advance + mCharSpacer);

        // If at end of row
        if( mWidth - 1 < mImage_l + ( advance ) )
        {
            mImage_m += ( mMaxHeight >> 6 ) + mCharSpacer;
            mImage_l = 0;
        }
        --mLeftBlankNum;

        Ogre::MemoryDataStream *dateStream = new Ogre::MemoryDataStream(mImageData, mWidth * mHeight * mPixelBytes);

        TexturePtr tex = TextureManager::getSingleton().loadRawData(
            mTextureName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::DataStreamPtr(dateStream),
            mWidth,
            mHeight,
            Ogre::PF_A8R8G8B8,
            Ogre::TEX_TYPE_2D,
            0);

        //Image img;
        //tex->convertToImage(img);
        //img.save("F:\\test.png");

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

    void Font::insertGlyphInfo(CodePoint id)
    {
        if(!hasBlankInTexture())    //has no space left in texture    
        {
            removeGlyph(getLessUseChar());
        }
        renderGlyphIntoTexture(id);

    }

    Font::CodePoint Font::getLessUseChar()
    {
        CodePointMap::iterator i = mCodePointMap.begin(), iend = mCodePointMap.end(), iless = mCodePointMap.begin();
        while(i != iend)
        {
            if(i->second.useCount < iless->second.useCount)
                iless = i;
            ++i;
        }
        return iless->second.codePoint;   
    }

    void Font::removeGlyph(CodePoint id)
    {
        CodePointMap::iterator it = mCodePointMap.find(id);
        if(it != mCodePointMap.end())
        {
            mImage_l = it->second.l;
            mImage_m = it->second.m;
            mCodePointMap.erase(it);
            ++mLeftBlankNum;
        }
        else
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Can not find CodePoint to remove", "Font::removeGlyph(CodePoint id)");
        }
    }
}