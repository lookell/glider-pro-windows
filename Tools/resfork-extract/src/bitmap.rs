#![allow(unused)]
use crate::res::RGBColor;
use crate::utils::WriteExt;
use std::io::{self, Write};

#[derive(Clone, Copy, Eq, PartialEq)]
pub struct RgbQuad {
    pub red: u8,
    pub green: u8,
    pub blue: u8,
}

impl RgbQuad {
    pub const BLACK: Self = Self::new(0x00, 0x00, 0x00);
    pub const WHITE: Self = Self::new(0xFF, 0xFF, 0xFF);
    pub const SIZE: u32 = 4;

    pub const fn new(red: u8, green: u8, blue: u8) -> Self {
        Self { red, green, blue }
    }

    pub fn write_to(self, mut writer: impl Write) -> io::Result<()> {
        writer.write_all(&[self.blue, self.green, self.red, 0x00])
    }
}

impl From<RgbQuad> for RGBColor {
    fn from(value: RgbQuad) -> RGBColor {
        let red = u16::from(value.red) * 0x0101;
        let green = u16::from(value.green) * 0x0101;
        let blue = u16::from(value.blue) * 0x0101;
        RGBColor::new(red, green, blue)
    }
}

impl From<RGBColor> for RgbQuad {
    fn from(value: RGBColor) -> RgbQuad {
        let red = (value.red >> 8) as u8;
        let green = (value.green >> 8) as u8;
        let blue = (value.blue >> 8) as u8;
        RgbQuad::new(red, green, blue)
    }
}

#[derive(Clone, Copy)]
pub struct BitmapFileHeader {
    pub type_: u16,
    pub size: u32,
    pub reserved1: u16,
    pub reserved2: u16,
    pub off_bits: u32,
}

impl BitmapFileHeader {
    pub const SIZE: u32 = 14;
    pub const TYPE: u16 = b'B' as u16 | (b'M' as u16) << 8;

    pub fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        writer.write_le_u16(self.type_)?;
        writer.write_le_u32(self.size)?;
        writer.write_le_u16(self.reserved1)?;
        writer.write_le_u16(self.reserved2)?;
        writer.write_le_u32(self.off_bits)?;
        Ok(())
    }
}

#[repr(u32)]
#[derive(Clone, Copy)]
pub enum Compression {
    Rgb = 0,
    Rle8 = 1,
    Rle4 = 2,
}

#[derive(Clone, Copy)]
pub struct BitmapInfoHeader {
    pub size: u32,
    pub width: i32,
    pub height: i32,
    pub planes: u16,
    pub bit_count: u16,
    pub compression: Compression,
    pub size_image: u32,
    pub x_pels_per_meter: i32,
    pub y_pels_per_meter: i32,
    pub clr_used: u32,
    pub clr_important: u32,
}

impl BitmapInfoHeader {
    pub const SIZE: u32 = 40;

    pub fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        writer.write_le_u32(self.size)?;
        writer.write_le_i32(self.width)?;
        writer.write_le_i32(self.height)?;
        writer.write_le_u16(self.planes)?;
        writer.write_le_u16(self.bit_count)?;
        writer.write_le_u32(self.compression as _)?;
        writer.write_le_u32(self.size_image)?;
        writer.write_le_i32(self.x_pels_per_meter)?;
        writer.write_le_i32(self.y_pels_per_meter)?;
        writer.write_le_u32(self.clr_used)?;
        writer.write_le_u32(self.clr_important)?;
        Ok(())
    }
}

#[repr(C)]
#[derive(Clone)]
pub struct BitmapInfo {
    header: BitmapInfoHeader,
    colors: Vec<RgbQuad>,
}

fn is_palette_valid_length(clut: &[RgbQuad]) -> bool {
    clut.is_empty() || clut.len() == 2 || clut.len() == 16 || clut.len() == 256
}

impl BitmapInfo {
    pub fn new(header: BitmapInfoHeader, colors: &[RgbQuad]) -> Self {
        assert!(is_palette_valid_length(&colors));
        let colors = colors.to_vec();
        Self { header, colors }
    }

    pub fn header(&self) -> &BitmapInfoHeader {
        &self.header
    }

    pub fn colors(&self) -> &[RgbQuad] {
        &self.colors
    }

    pub fn byte_size(&self) -> u32 {
        BitmapInfoHeader::SIZE + RgbQuad::SIZE * (self.colors.len() as u32)
    }

    pub fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        self.header.write_to(&mut writer)?;
        for rgb in self.colors.iter() {
            rgb.write_to(&mut writer)?;
        }
        Ok(())
    }
}

const fn bits_length(stride: usize, height: u16) -> usize {
    stride * height as usize
}

const fn byte_stride(width: u16, depth: u16) -> usize {
    let bit_width = (width as usize) * (depth as usize);
    let aligned_bit_width = (bit_width + 31) & !31;
    aligned_bit_width / 8
}

const fn bitmap_size(width: u16, height: u16, depth: u16) -> usize {
    byte_stride(width, depth) * (height as usize)
}

fn zeroed_bitmap(width: u16, height: u16, depth: u16) -> Vec<u8> {
    vec![0x00; bitmap_size(width, height, depth)]
}

pub trait Bitmap: Sized {
    type Pixel: Copy;
    const BIT_COUNT: u16;
    const BLACK: Self::Pixel;

    fn new(width: u16, height: u16) -> Self;
    fn width(&self) -> u16;
    fn height(&self) -> u16;
    fn bits(&self) -> &[u8];
    fn palette(&self) -> &[RgbQuad];
    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I);
    fn get_pixel(&self, x: u16, y: u16) -> Self::Pixel;
    fn set_pixel(&mut self, x: u16, y: u16, pixel: Self::Pixel);

    fn bit_count(&self) -> u16 {
        Self::BIT_COUNT
    }

    fn apply_mask(&mut self, mask: &BitmapOne) {
        for y in 0..mask.height() {
            for x in 0..mask.width() {
                if mask.get_pixel(x, y) == 1 {
                    self.set_pixel(x, y, Self::BLACK);
                }
            }
        }
    }

    fn info_header(&self) -> BitmapInfoHeader {
        BitmapInfoHeader {
            size: BitmapInfoHeader::SIZE,
            width: self.width().into(),
            height: self.height().into(),
            planes: 1,
            bit_count: self.bit_count(),
            compression: Compression::Rgb,
            size_image: self.bits().len() as _,
            x_pels_per_meter: 0,
            y_pels_per_meter: 0,
            clr_used: 0,
            clr_important: 0,
        }
    }

    fn info(&self) -> BitmapInfo {
        BitmapInfo::new(self.info_header(), self.palette())
    }

    fn write_bmp_file(&self, mut writer: impl Write) -> io::Result<()> {
        let info = self.info();
        let bits_offset = BitmapFileHeader::SIZE + info.byte_size();
        let file_header = BitmapFileHeader {
            type_: BitmapFileHeader::TYPE,
            size: bits_offset + info.header().size_image,
            reserved1: 0,
            reserved2: 0,
            off_bits: bits_offset,
        };
        file_header.write_to(&mut writer)?;
        info.write_to(&mut writer)?;
        writer.write_all(self.bits())?;
        Ok(())
    }
}

#[derive(Clone)]
pub struct BitmapOne {
    width: u16,
    height: u16,
    bits: Vec<u8>,
    palette: Vec<RgbQuad>,
}

impl Bitmap for BitmapOne {
    type Pixel = u8;
    const BIT_COUNT: u16 = 1;
    const BLACK: Self::Pixel = 0;

    fn new(width: u16, height: u16) -> Self {
        Self {
            width,
            height,
            bits: zeroed_bitmap(width, height, Self::BIT_COUNT),
            palette: vec![RgbQuad::BLACK; 1 << Self::BIT_COUNT],
        }
    }

    fn width(&self) -> u16 {
        self.width
    }

    fn height(&self) -> u16 {
        self.height
    }

    fn bits(&self) -> &[u8] {
        &self.bits
    }

    fn palette(&self) -> &[RgbQuad] {
        &self.palette
    }

    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = src)
    }

    fn get_pixel(&self, x: u16, y: u16) -> u8 {
        if (x >= self.width) || (y >= self.height) {
            return 0;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x / 8);
        let shift = 7 - (x % 8);
        let mask = 0x1 << shift;
        (self.bits[byte_offset] & mask) >> shift
    }

    fn set_pixel(&mut self, x: u16, y: u16, pixel: u8) {
        if (x >= self.width) || (y >= self.height) {
            return;
        }
        let pixel = pixel & 0x1;
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x / 8);
        let shift = 7 - (x % 8);
        let mask = 0x1 << shift;
        self.bits[byte_offset] &= !mask;
        self.bits[byte_offset] |= pixel << shift;
    }
}

#[derive(Clone)]
pub struct BitmapFour {
    width: u16,
    height: u16,
    bits: Vec<u8>,
    palette: Vec<RgbQuad>,
}

impl Bitmap for BitmapFour {
    type Pixel = u8;
    const BIT_COUNT: u16 = 4;
    const BLACK: Self::Pixel = 0;

    fn new(width: u16, height: u16) -> Self {
        Self {
            width,
            height,
            bits: zeroed_bitmap(width, height, Self::BIT_COUNT),
            palette: vec![RgbQuad::BLACK; 1 << Self::BIT_COUNT],
        }
    }

    fn width(&self) -> u16 {
        self.width
    }

    fn height(&self) -> u16 {
        self.height
    }

    fn bits(&self) -> &[u8] {
        &self.bits
    }

    fn palette(&self) -> &[RgbQuad] {
        &self.palette
    }

    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = src)
    }

    fn get_pixel(&self, x: u16, y: u16) -> u8 {
        if (x >= self.width) || (y >= self.height) {
            return 0;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x / 2);
        let shift = 4 * (1 - (x % 2));
        let mask = 0xF << shift;
        (self.bits[byte_offset] & mask) >> shift
    }

    fn set_pixel(&mut self, x: u16, y: u16, pixel: u8) {
        if (x >= self.width) || (y >= self.height) {
            return;
        }
        let pixel = pixel & 0xF;
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x / 2);
        let shift = 4 * (1 - (x % 2));
        let mask = 0xF << shift;
        self.bits[byte_offset] &= !mask;
        self.bits[byte_offset] |= pixel << shift;
    }
}

#[derive(Clone)]
pub struct BitmapEight {
    width: u16,
    height: u16,
    bits: Vec<u8>,
    palette: Vec<RgbQuad>,
}

impl Bitmap for BitmapEight {
    type Pixel = u8;
    const BIT_COUNT: u16 = 8;
    const BLACK: Self::Pixel = 0;

    fn new(width: u16, height: u16) -> Self {
        Self {
            width,
            height,
            bits: zeroed_bitmap(width, height, Self::BIT_COUNT),
            palette: vec![RgbQuad::BLACK; 1 << Self::BIT_COUNT],
        }
    }

    fn width(&self) -> u16 {
        self.width
    }

    fn height(&self) -> u16 {
        self.height
    }

    fn bits(&self) -> &[u8] {
        &self.bits
    }

    fn palette(&self) -> &[RgbQuad] {
        &self.palette
    }

    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = src)
    }

    fn get_pixel(&self, x: u16, y: u16) -> u8 {
        if (x >= self.width) || (y >= self.height) {
            return 0;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x);
        self.bits[byte_offset]
    }

    fn set_pixel(&mut self, x: u16, y: u16, pixel: u8) {
        if (x >= self.width) || (y >= self.height) {
            return;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + usize::from(x);
        self.bits[byte_offset] = pixel;
    }
}

#[derive(Clone)]
pub struct BitmapSixteen {
    width: u16,
    height: u16,
    bits: Vec<u8>,
}

impl Bitmap for BitmapSixteen {
    type Pixel = RgbQuad;
    const BIT_COUNT: u16 = 16;
    const BLACK: Self::Pixel = RgbQuad::BLACK;

    fn new(width: u16, height: u16) -> Self {
        Self {
            width,
            height,
            bits: zeroed_bitmap(width, height, Self::BIT_COUNT),
        }
    }

    fn width(&self) -> u16 {
        self.width
    }

    fn height(&self) -> u16 {
        self.height
    }

    fn bits(&self) -> &[u8] {
        &self.bits
    }

    fn palette(&self) -> &[RgbQuad] {
        &[]
    }

    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I) {}

    fn get_pixel(&self, x: u16, y: u16) -> RgbQuad {
        if (x >= self.width) || (y >= self.height) {
            return RgbQuad::BLACK;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + 2 * usize::from(x);
        let word = u16::from_le_bytes([self.bits[byte_offset], self.bits[byte_offset + 1]]);
        let red = ((word >> 10) & 0x1F) as u8;
        let green = ((word >> 5) & 0x1F) as u8;
        let blue = (word & 0x1F) as u8;
        RgbQuad::new(
            red << 3 | red >> 2,
            green << 3 | green >> 2,
            blue << 3 | blue >> 2,
        )
    }

    fn set_pixel(&mut self, x: u16, y: u16, pixel: RgbQuad) {
        if (x >= self.width) || (y >= self.height) {
            return;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + 2 * usize::from(x);
        let mut word = 0;
        word |= u16::from(pixel.red >> 3) << 10;
        word |= u16::from(pixel.green >> 3) << 5;
        word |= u16::from(pixel.blue >> 3);
        let bytes = word.to_le_bytes();
        self.bits[byte_offset] = bytes[0];
        self.bits[byte_offset + 1] = bytes[1];
    }
}

#[derive(Clone)]
pub struct BitmapTwentyFour {
    width: u16,
    height: u16,
    bits: Vec<u8>,
}

impl Bitmap for BitmapTwentyFour {
    type Pixel = RgbQuad;
    const BIT_COUNT: u16 = 24;
    const BLACK: Self::Pixel = RgbQuad::BLACK;

    fn new(width: u16, height: u16) -> Self {
        Self {
            width,
            height,
            bits: zeroed_bitmap(width, height, Self::BIT_COUNT),
        }
    }

    fn width(&self) -> u16 {
        self.width
    }

    fn height(&self) -> u16 {
        self.height
    }

    fn bits(&self) -> &[u8] {
        &self.bits
    }

    fn palette(&self) -> &[RgbQuad] {
        &[]
    }

    fn set_palette<I: IntoIterator<Item = RgbQuad>>(&mut self, new_palette: I) {}

    fn get_pixel(&self, x: u16, y: u16) -> RgbQuad {
        if (x >= self.width) || (y >= self.height) {
            return RgbQuad::BLACK;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + 3 * usize::from(x);
        let blue = self.bits[byte_offset];
        let green = self.bits[byte_offset + 1];
        let red = self.bits[byte_offset + 2];
        RgbQuad::new(red, green, blue)
    }

    fn set_pixel(&mut self, x: u16, y: u16, pixel: RgbQuad) {
        if (x >= self.width) || (y >= self.height) {
            return;
        }
        let y = (self.height - 1) - y;
        let stride = byte_stride(self.width, Self::BIT_COUNT);
        let row_start = stride * usize::from(y);
        let byte_offset = row_start + 3 * usize::from(x);
        self.bits[byte_offset] = pixel.blue;
        self.bits[byte_offset + 1] = pixel.green;
        self.bits[byte_offset + 2] = pixel.red;
    }
}
