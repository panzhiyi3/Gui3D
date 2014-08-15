#pragma once

#include <map>
#include <vector>
#include <ft2build.h>
#include "Ogre.h"
#include "FontManager.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Gorilla
{
    class FreeTypeFont : public Gorilla::Font
    {
    public:
        FreeTypeFont(int textureSize = 1024, int ttfSize = 20.0, int ttfResolution = 72, bool antiAliased = false);

        ~FreeTypeFont();

        virtual void load(const std::string &name, const std::string &fontName);

        virtual void unLoad();

        virtual void addCodePointRange(const CodePointRange& range);

        virtual void clearCodePointRanges();

        virtual inline const CodePointRangeList& getCodePointRangeList() const;

        virtual inline const UVRect& getGlyphTexCoords(CodePoint id) const;

        //virtual inline void setGlyphTexCoords(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel, unsigned int u2Pixel, unsigned int v2Pixel, float textureAspect);

        virtual inline float getGlyphAspectRatio(CodePoint id) const;

        virtual inline void setGlyphAspectRatio(CodePoint id, float ratio);

        virtual inline void setGlyphInfo(CodePoint id, unsigned int u1Pixel, unsigned int v1Pixel, unsigned int u2Pixel, unsigned int v2Pixel, unsigned int advance, float textureAspect);

        virtual const GlyphInfo *getGlyphInfo(CodePoint id) const;

        virtual void insertGlyphInfo(CodePoint id);

        virtual bool hasBlankInTexture() const;

        virtual void renderGlyphIntoTexture(CodePoint id);

        virtual CodePoint getLessUseChar();

        virtual void removeGlyph(CodePoint id);
    };
}
