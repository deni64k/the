#include <algorithm>
#include <cstring>
#include <fstream>
#include <map>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "lib/log.hxx"
#include "lib/ui/fonts.hxx"

#ifdef DEBUG
# undef DEBUG
#endif
#define DEBUG(...) if (false) the::streams::Null

namespace the::ui {

static std::map<FT_ULong, FT_GlyphSlot> Glyphs;

std::optional<FT_GlyphSlot> LoadGlyph(FT_ULong charcode) {
  auto iter = Glyphs.lower_bound(charcode);
  if (iter != std::end(Glyphs) && iter->first == charcode) {
    return iter->second;
  }

  // FT_Load_Glyph

  return {};
}

template <typename T>
void DrawChar(Image<T> &bmp, FT_GlyphSlot const &slot, std::size_t penX, std::size_t penY) {
  DEBUG() << "DrawChar(penX=" << penX << ", penY=" << penY;
  // std::uint8_t const grey = 255;
  auto const &bitmap = slot->bitmap;

  // penX += slot->bitmap_left;
  // penY += slot->bitmap_top;

  std::uint8_t const *buf = bitmap.buffer;
  decltype(auto)      row = buf;
  for (unsigned y = 0; y < bitmap.rows; ++y, row = buf += bitmap.pitch) {
    for (unsigned x = 0; x < bitmap.width; ++x, ++row) {
      // DEBUG() << "static_cast<unsigned int>(*row) = " << static_cast<unsigned int>(*row);
      // DEBUG() << "penX + x = " << penX + x;
      // DEBUG() << "penY + y = " << penY + y;
      bmp[penX + x][penY + y] |= static_cast<std::uint8_t>(*row);
    }
  }
}

template <typename T>
void DumpBmp(Image<T> const &bmp, char const *fname) {
  std::ofstream os{fname};

  int const width  = bmp.Width();
  int const height = bmp.Height();

  os << "P2\n" << width << ' ' << height << '\n';
  os << "255\n";
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      os << static_cast<unsigned int>(bmp[x][y]) << ' ';
    }
    os << '\n';
  }
}

// TODO:
// * No RTL language supported.
// * Fallback to a broader but uglier font if a character is not found.
// * Handle negative bitmap_left, and rendering characters by negative coordinates, gracefully.
// * Enable the alpha channel.
Fallible<Image<std::uint8_t[4]>> RenderFont(gsl::span<char const> text) {
  // https://www.freetype.org/freetype2/docs/tutorial/step1.html
  FT_Library library;
  FT_Error err;

  if (err = FT_Init_FreeType(&library); err != 0) {
    return {Error{"FT_Init_FreeType failed"}};
  }

  FT_Face face;
  FT_Long faceIndex = 0;
  
  // char const fontPath[] = "/System/Library/Fonts/Menlo.ttc";
  // char const fontPath[] = "/Library/Fonts/Times New Roman Italic.ttf";
  char const fontPath[] = "/Library/Fonts/Arial Unicode.ttf";
  switch (err = FT_New_Face(library,
                            fontPath,
                            faceIndex,
                            &face); err) {
    case 0:
      // No error.
      break;
    case FT_Err_Unknown_File_Format:
      return {Error{"FT_New_Face failed: unknown font format"}};
    default:
      return {Error{"FT_New_Face failed"}};
  }

  bool const useKerning = FT_HAS_KERNING(face);
  
  DEBUG() << "text = " << std::quoted(std::string(text.data(), text.size()));

  DEBUG() << "face->num_glyphs          = " << face->num_glyphs;
  DEBUG() << "face->face_flags          = " << std::hex << face->num_glyphs << std::dec;
  DEBUG() << "face->units_per_EM        = " << face->units_per_EM;
  DEBUG() << "face->num_fixed_sizes     = " << face->num_fixed_sizes;
  DEBUG() << "face->bbox.xMin           = " << face->bbox.xMin;
  DEBUG() << "face->bbox.yMin           = " << face->bbox.yMin;
  DEBUG() << "face->bbox.xMax           = " << face->bbox.xMax;
  DEBUG() << "face->bbox.yMax           = " << face->bbox.yMax;
  DEBUG() << "face->ascender            = " << face->ascender;
  DEBUG() << "face->descender           = " << face->descender;
  DEBUG() << "face->height              = " << face->height;
  DEBUG() << "face->max_advance_width   = " << face->max_advance_width;
  DEBUG() << "face->max_advance_height  = " << face->max_advance_height;
  DEBUG() << "face->underline_position  = " << face->underline_position;
  DEBUG() << "face->underline_thickness = " << face->underline_thickness;

  if (err = FT_Set_Char_Size(face,
                             0,       // char_width in 1/64th of points
                             16*64,   // char_height in 1/64th of points
                             300,     // horizontal device resolution
                             300);    // vertical device resolution
      err != 0) {
    return {Error{"FT_Set_Char_Size failed"}};
  }

  int penX = 0;
  int penY = 0;
  std::size_t const maxWidth  = face->size->metrics.max_advance >> 6;
  std::size_t const maxHeight = face->size->metrics.height >> 6;
  std::size_t const textWidth = maxWidth * text.size();

  DEBUG() << "useKerning = " << useKerning;
  DEBUG() << "maxWidth   = " << maxWidth;
  DEBUG() << "maxHeight  = " << maxHeight;
  DEBUG() << "textWidth   = " << textWidth;

  unsigned int const grey = 0;
  Image<std::uint8_t> bmp{textWidth, maxHeight};
  bmp.fill(grey);
  
  FT_Long glyphIndexPrev = 0;

  for (FT_ULong const ch : text) {
    if (ch == '\0') {
      break;
    }
    
    FT_Long const glyphIndex = FT_Get_Char_Index(face, ch);

    DEBUG() << "penX = " << penX;
    DEBUG() << "penY = " << penY;
    DEBUG() << "charcode   = " << std::quoted(std::string(1, static_cast<char>(ch)));
    DEBUG() << "glyphIndex = " << glyphIndex;
    
    // https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_LOAD_XXX
    if (err = FT_Load_Glyph(face, glyphIndex, FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER); err != 0) {
      return {Error{"FT_Load_Glyph failed"}};
    }

    auto const &slot   = face->glyph;
    auto const &bitmap = slot->bitmap;

    DEBUG() << "slot−>format      = " << std::hex << slot->format << std::dec;
    DEBUG() << "slot->bitmap_left = " << slot->bitmap_left;
    DEBUG() << "slot->bitmap_top  = " << slot->bitmap_top;
    DEBUG() << "slot−>advance.x   = " << slot->advance.x;
    DEBUG() << "slot−>advance.y   = " << slot->advance.y;
    DEBUG() << "bitmap.num_grays  = " << bitmap.num_grays;
    DEBUG() << "bitmap.rows       = " << bitmap.rows;
    DEBUG() << "bitmap.width      = " << bitmap.width;
    DEBUG() << "bitmap.pitch      = " << bitmap.pitch;

    if (glyphIndex && glyphIndexPrev) {
      if (useKerning) {
        FT_Vector delta;

        if (err = FT_Get_Kerning(face, glyphIndexPrev, glyphIndex, FT_KERNING_DEFAULT, &delta); err != 0) {
          return {Error{"FT_Get_Kerning failed"}};
        }

        DEBUG() << "delta = (" << (delta.x >> 6) << ", " << (delta.y >> 6) << ')';
      
        penX += delta.x >> 6;
        penY += delta.y >> 6;
      }
    } else {
      if (slot->bitmap_left < 0 && penX < -slot->bitmap_left) {
        penX += -slot->bitmap_left;
      }
      if (slot->bitmap_top < 0 && penY < -slot->bitmap_top) {
        penY += -slot->bitmap_top;
      }
    }
    DrawChar(bmp, slot,
             penX + slot->bitmap_left,
             penY + (face->size->metrics.ascender >> 6) - slot->bitmap_top);

    // increment pen position
    penX += slot->advance.x >> 6;
    penY += slot->advance.y >> 6;

    glyphIndexPrev = glyphIndex;
  }

  // DumpBmp(bmp, "font.ppm");

  if (err = FT_Done_Face(face); err != 0) {
    return {Error{"FT_Done_Face failed"}};
  }

  if (err = FT_Done_FreeType(library); err != 0) {
    return {Error{"FT_Done_FreeType failed"}};
  }

  Image<std::uint8_t[4]> resultBmp{static_cast<std::size_t>(penX), bmp.Height()};
  for (std::size_t row = 0, opp = resultBmp.Height() - 1; row < resultBmp.Height(); ++row, --opp) {
    for (std::size_t col = 0; col < resultBmp.Width(); ++col) {
      resultBmp[col][row][0] = 0xff;
      resultBmp[col][row][1] = 0xff;
      resultBmp[col][row][2] = 0xff;
      resultBmp[col][row][3] = 0xff;

      resultBmp[col][row][3] = bmp[col][opp];

      resultBmp[col][row][0] = bmp[col][opp];
      resultBmp[col][row][1] = bmp[col][opp];
      resultBmp[col][row][2] = bmp[col][opp];
      resultBmp[col][row][3] = bmp[col][opp];
    }
  }
  // Image<std::uint8_t> resultBmp{static_cast<std::size_t>(penX), bmp.Height()};
  // // Image<std::uint8_t> resultBmp{bmp.Width(), bmp.Height()};
  // for (std::size_t col = 0; col < resultBmp.Width(); ++col) {
  //   for (std::size_t row = 0, opp = resultBmp.Height() - 1; row < resultBmp.Height(); ++row, --opp) {
  //     resultBmp[col][row] = bmp[col][opp];
  //   }
  // }

  // DumpBmp(resultBmp, "font.ppm");

  return {std::move(resultBmp)};
  // return {std::move(bmp)};
}

}
