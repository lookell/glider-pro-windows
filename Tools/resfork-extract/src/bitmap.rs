#![allow(unused)]
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
    pub const SIZE: u32 = 4;

    pub const fn new(red: u8, green: u8, blue: u8) -> Self {
        Self { red, green, blue }
    }

    pub fn write_to(self, mut writer: impl Write) -> io::Result<()> {
        writer.write_all(&[self.blue, self.green, self.red, 0x00])
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

pub trait Bitmap {
    const BIT_COUNT: u16;

    fn new(width: u16, height: u16) -> Self;
    fn width(&self) -> u16;
    fn height(&self) -> u16;
    fn bits(&self) -> &[u8];
    fn palette(&self) -> &[RgbQuad];
    fn set_palette(&mut self, new_palette: &[RgbQuad]);

    fn bit_count(&self) -> u16 {
        Self::BIT_COUNT
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
    const BIT_COUNT: u16 = 1;

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

    fn set_palette(&mut self, new_palette: &[RgbQuad]) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = *src)
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
    const BIT_COUNT: u16 = 4;

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

    fn set_palette(&mut self, new_palette: &[RgbQuad]) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = *src)
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
    const BIT_COUNT: u16 = 8;

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

    fn set_palette(&mut self, new_palette: &[RgbQuad]) {
        self.palette
            .iter_mut()
            .zip(new_palette)
            .for_each(|(dst, src)| *dst = *src)
    }
}

#[derive(Clone)]
pub struct BitmapSixteen {
    width: u16,
    height: u16,
    bits: Vec<u8>,
}

impl Bitmap for BitmapSixteen {
    const BIT_COUNT: u16 = 16;

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

    fn set_palette(&mut self, new_palette: &[RgbQuad]) {}
}

#[derive(Clone)]
pub struct BitmapTwentyFour {
    width: u16,
    height: u16,
    bits: Vec<u8>,
}

impl Bitmap for BitmapTwentyFour {
    const BIT_COUNT: u16 = 24;

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

    fn set_palette(&mut self, new_palette: &[RgbQuad]) {}
}