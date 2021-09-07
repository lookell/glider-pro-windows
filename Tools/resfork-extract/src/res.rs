#![allow(dead_code)]
#![allow(nonstandard_style)]

use crate::bitmap::{
    Bitmap, BitmapEight, BitmapFour, BitmapOne, BitmapSixteen, BitmapTwentyFour, RgbQuad,
};
use crate::mac_roman;
use crate::utils::ReadExt;
use std::io::{self, Read};

pub mod alert; // 'ALRT'
pub mod animated_cursor; // 'acur'
pub mod bundle; // 'BNDL'
pub mod color_cursor; // 'crsr'
pub mod color_icon; // 'cicn'
pub mod color_table; // 'clut'
pub mod control; // 'CNTL'
pub mod control_color_table; // 'cctb'
pub mod cursor; // 'CURS'
pub mod dialog; // 'DLOG'
pub mod dialog_color_table; // 'dctb'
pub mod file_reference; // 'FREF'
pub mod icon; // 'ICON'
pub mod icon_list; // 'ICN#'
pub mod item_list; // 'DITL'
pub mod large_4bit_icon; // 'icl4'
pub mod large_8bit_icon; // 'icl8'
pub mod menu; // 'MENU'
pub mod menu_color_table; // 'mctb'
pub mod pattern_list; // 'PAT#'
pub mod picture; // 'PICT'
pub mod small_4bit_icon; // 'ics4'
pub mod small_8bit_icon; // 'ics8'
pub mod small_icon_list; // 'ics#'
pub mod sound; // 'snd '
pub mod string_list; // 'STR#'
pub mod text; // 'TEXT'
pub mod version; // 'vers'
pub mod window; // 'WIND'
pub mod window_color_table; // 'wctb'

pub fn pix_to_xdlu(pix: i32) -> i32 {
    const MS_SHELL_DLG_BASE_X: f64 = 6.0;
    (f64::from(pix) * 4.0 / MS_SHELL_DLG_BASE_X).round() as i32
}

pub fn pix_to_ydlu(pix: i32) -> i32 {
    const MS_SHELL_DLG_BASE_Y: f64 = 13.0;
    (f64::from(pix) * 8.0 / MS_SHELL_DLG_BASE_Y).round() as i32
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct Point {
    pub v: i16,
    pub h: i16,
}

impl Point {
    pub fn new(h: i16, v: i16) -> Self {
        Self { v, h }
    }

    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            v: reader.read_be_i16()?,
            h: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct Rect {
    pub top: i16,
    pub left: i16,
    pub bottom: i16,
    pub right: i16,
}

impl Rect {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            top: reader.read_be_i16()?,
            left: reader.read_be_i16()?,
            bottom: reader.read_be_i16()?,
            right: reader.read_be_i16()?,
        })
    }

    pub fn width(self) -> i32 {
        i32::from(self.right) - i32::from(self.left)
    }

    pub fn height(self) -> i32 {
        i32::from(self.bottom) - i32::from(self.top)
    }
}

pub fn read_pstring(mut reader: impl Read) -> io::Result<String> {
    let length = reader.read_be_u8()?;
    let mut buffer = vec![0x00; length.into()];
    reader.read_exact(buffer.as_mut_slice())?;
    Ok(buffer.into_iter().map(mac_roman::decode).collect())
}

#[derive(Clone, Copy, Eq, PartialEq)]
pub struct RGBColor {
    pub red: u16,
    pub green: u16,
    pub blue: u16,
}

impl RGBColor {
    pub const fn new(red: u16, green: u16, blue: u16) -> Self {
        Self { red, green, blue }
    }

    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            red: reader.read_be_u16()?,
            green: reader.read_be_u16()?,
            blue: reader.read_be_u16()?,
        })
    }
}

#[derive(Clone, Copy)]
pub struct ColorSpec {
    pub value: i16,
    pub rgb: RGBColor,
}

impl ColorSpec {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            value: reader.read_be_i16()?,
            rgb: RGBColor::read_from(&mut reader)?,
        })
    }
}

#[derive(Clone, Default)]
pub struct ColorTable {
    pub ctSeed: i32,
    pub ctFlags: i16,
    pub ctTable: Vec<ColorSpec>,
}

impl ColorTable {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let ctSeed = reader.read_be_i32()?;
        let ctFlags = reader.read_be_i16()?;
        let ctSize = reader.read_be_i16()?.wrapping_add(1);
        let mut ctTable = Vec::with_capacity((ctSize.max(0) as u16).into());
        for _ in 0..ctSize {
            ctTable.push(ColorSpec::read_from(&mut reader)?);
        }
        Ok(Self {
            ctSeed,
            ctFlags,
            ctTable,
        })
    }
}

// Everything that reads bitmap data in this converter reverses the palette
// to conform to Windows conventions. In Windows, the lowest color palette
// index is generally black, and the highest index is generally white. This
// is the opposite of the Macintosh's conventions, where the lowest color
// palette index is generally white, and the highest index is generally
// black.
//
// To properly implement this palette reversal, the color indices in a
// pixmap must also be tweaked to refer to the correct color. Where a pixel
// was once index 0, it will be changed to index 255 (for an 8-bit pixmap).
// For each pixmap depth, the color indices are converted as follows:
//
// * 1-bit pixmap: (1 - idx)
// * 2-bit pixmap: (3 - idx)
// * 4-bit pixmap: (15 - idx)
// * 8-bit pixmap: (255 - idx)
//
// The reversal also works to convert the Macintosh OR masks into Windows
// AND masks, for icons and cursors. This conveniently means that the same
// function can be used to read both a monochrome image and mask for an
// icon or cursor.

pub fn read_1bit_bitmap_data(bitmap: &mut BitmapOne, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        (bitmap.width() + 7) / 8
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        let y = y as u16;
        for (x_base, packed) in row.iter().copied().enumerate() {
            let x_base = 8 * x_base as u16;
            bitmap.set_pixel(x_base, y, ((packed & 0x80) == 0).into());
            bitmap.set_pixel(x_base + 1, y, ((packed & 0x40) == 0).into());
            bitmap.set_pixel(x_base + 2, y, ((packed & 0x20) == 0).into());
            bitmap.set_pixel(x_base + 3, y, ((packed & 0x10) == 0).into());
            bitmap.set_pixel(x_base + 4, y, ((packed & 0x08) == 0).into());
            bitmap.set_pixel(x_base + 5, y, ((packed & 0x04) == 0).into());
            bitmap.set_pixel(x_base + 6, y, ((packed & 0x02) == 0).into());
            bitmap.set_pixel(x_base + 7, y, ((packed & 0x01) == 0).into());
        }
    }
}

pub fn read_mask_bitmap_data(bitmap: &mut BitmapOne, data: &[u8], row_bytes: u16) {
    read_1bit_bitmap_data(bitmap, data, row_bytes)
}

pub fn read_2bit_bitmap_data(bitmap: &mut BitmapFour, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        (bitmap.width() + 3) / 4
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        let y = y as u16;
        for (x_base, packed) in row.iter().copied().enumerate() {
            let x_base = 4 * x_base as u16;
            bitmap.set_pixel(x_base, y, 15 - (packed / 64 % 4));
            bitmap.set_pixel(x_base + 1, y, 15 - (packed / 16 % 4));
            bitmap.set_pixel(x_base + 2, y, 15 - (packed / 4 % 4));
            bitmap.set_pixel(x_base + 3, y, 15 - (packed % 4));
        }
    }
}

pub fn read_4bit_bitmap_data(bitmap: &mut BitmapFour, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        (bitmap.width() + 1) / 2
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        let y = y as u16;
        for (x_base, packed) in row.iter().copied().enumerate() {
            let x_base = 2 * x_base as u16;
            bitmap.set_pixel(x_base, y, 15 - (packed / 16));
            bitmap.set_pixel(x_base + 1, y, 15 - (packed % 16));
        }
    }
}

pub fn read_8bit_bitmap_data(bitmap: &mut BitmapEight, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        bitmap.width()
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        let y = y as u16;
        for (x, idx) in row.iter().copied().enumerate() {
            let x = x as u16;
            bitmap.set_pixel(x, y, 255 - idx);
        }
    }
}

pub fn read_16bit_bitmap_data(bitmap: &mut BitmapSixteen, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        2 * bitmap.width()
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        for (x, pair) in row.chunks_exact(2).enumerate() {
            let red = 8 * ((pair[0] & 0x7A) >> 2);
            let green = 8 * (((pair[0] & 0x03) << 3) | ((pair[1] & 0xE0) >> 5));
            let blue = 8 * (pair[1] & 0x1F);
            bitmap.set_pixel(x as u16, y as u16, RgbQuad::new(red, green, blue));
        }
    }
}

pub fn read_32bit_bitmap_data(bitmap: &mut BitmapTwentyFour, data: &[u8], row_bytes: u16) {
    let row_bytes = if row_bytes == 0 {
        4 * bitmap.width()
    } else {
        row_bytes
    };
    for (y, row) in data.chunks_exact(row_bytes.into()).enumerate() {
        for (x, quad) in row.chunks_exact(4).enumerate() {
            let red = quad[1];
            let green = quad[2];
            let blue = quad[3];
            bitmap.set_pixel(x as u16, y as u16, RgbQuad::new(red, green, blue));
        }
    }
}
