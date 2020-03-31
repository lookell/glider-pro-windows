#![allow(unused)]

use crate::bitmap::{Bitmap, BitmapInfo, BitmapInfoHeader, BitmapOne};
use crate::res::Point;
use crate::utils::WriteExt;
use std::io::{self, Write};
use std::iter;

#[derive(Clone, Default)]
pub struct IconFile {
    entries: Vec<IconFileEntry>,
}

#[derive(Clone)]
struct IconFileEntry {
    dir_entry: IconDirEntry,
    bitmap_info: BitmapInfo,
    image_bits: Vec<u8>,
    mask_bits: Vec<u8>,
}

impl IconFile {
    pub fn new() -> Self {
        Self {
            entries: Vec::new(),
        }
    }

    fn next_image_offset(&self) -> u32 {
        assert_ne!(self.entries.len(), usize::from(u16::max_value()));
        let mut next_offset = DirectoryHeader::SIZE;
        next_offset += IconDirEntry::SIZE * (self.entries.len() + 1) as u32;
        for entry in self.entries.iter() {
            next_offset += entry.dir_entry.bytes_in_res;
        }
        next_offset
    }

    pub fn add_entry(&mut self, mut image: impl Bitmap, mask: BitmapOne) {
        assert_ne!(image.bit_count(), 24, "24-bit images are not supported");
        assert_ne!(self.entries.len(), usize::from(u16::max_value()));
        assert_eq!(image.width(), mask.width());
        assert_eq!(image.height(), mask.height());
        assert!(image.width() <= 256);
        assert!(image.height() <= 256);
        for entry in self.entries.iter_mut() {
            entry.dir_entry.image_offset += IconDirEntry::SIZE;
        }
        // Apply the mask to the image, to make sure that the transparent
        // parts will actually be transparent.
        image.apply_mask(&mask);
        // The height and image data size are adjusted to compensate
        // for the hybrid bitmap data.
        let mut info_header = image.info_header();
        info_header.height *= 2;
        info_header.size_image += mask.bits().len() as u32;
        self.entries.push(IconFileEntry {
            dir_entry: IconDirEntry {
                image_offset: self.next_image_offset(),
                ..IconDirEntry::new(&image, &mask)
            },
            bitmap_info: BitmapInfo::new(info_header, image.palette()),
            image_bits: image.bits().to_vec(),
            mask_bits: mask.bits().to_vec(),
        });
    }

    fn directory_header(&self) -> DirectoryHeader {
        DirectoryHeader {
            reserved: 0,
            res_type: RES_ICON,
            res_count: self.entries.len() as _,
        }
    }

    fn directory_entries(&self) -> impl Iterator<Item = &IconDirEntry> {
        self.entries.iter().map(|entry| &entry.dir_entry)
    }

    pub fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        self.directory_header().write_to(&mut writer)?;
        for dir_entry in self.directory_entries() {
            dir_entry.write_to(&mut writer)?;
        }
        for entry in self.entries.iter() {
            entry.bitmap_info.write_to(&mut writer)?;
            writer.write_all(&entry.image_bits)?;
            writer.write_all(&entry.mask_bits)?;
        }
        Ok(())
    }
}

#[derive(Clone, Default)]
pub struct CursorFile {
    entries: Vec<CursorFileEntry>,
}

#[derive(Clone)]
struct CursorFileEntry {
    dir_entry: CursorDirEntry,
    bitmap_info: BitmapInfo,
    image_bits: Vec<u8>,
    mask_bits: Vec<u8>,
}

impl CursorFile {
    pub fn new() -> Self {
        Self {
            entries: Vec::new(),
        }
    }

    fn next_image_offset(&self) -> u32 {
        assert_ne!(self.entries.len(), usize::from(u16::max_value()));
        let mut next_offset = DirectoryHeader::SIZE;
        next_offset += CursorDirEntry::SIZE * (self.entries.len() + 1) as u32;
        for entry in self.entries.iter() {
            next_offset += entry.dir_entry.bytes_in_res;
        }
        next_offset
    }

    pub fn add_entry(&mut self, mut image: impl Bitmap, mask: BitmapOne, hotspot: Point) {
        assert_ne!(image.bit_count(), 24, "24-bit images are not supported");
        assert_ne!(self.entries.len(), usize::from(u16::max_value()));
        assert_eq!(image.width(), mask.width());
        assert_eq!(image.height(), mask.height());
        assert!(image.width() <= 256);
        assert!(image.height() <= 256);
        for entry in self.entries.iter_mut() {
            entry.dir_entry.image_offset += CursorDirEntry::SIZE;
        }
        // Apply the mask to the image, to make sure that the transparent
        // parts will actually be transparent.
        image.apply_mask(&mask);
        // The height and image data size are adjusted to compensate
        // for the hybrid bitmap data.
        let mut info_header = image.info_header();
        info_header.height *= 2;
        info_header.size_image += mask.bits().len() as u32;
        self.entries.push(CursorFileEntry {
            dir_entry: CursorDirEntry {
                image_offset: self.next_image_offset(),
                ..CursorDirEntry::new(&image, &mask, hotspot)
            },
            bitmap_info: BitmapInfo::new(info_header, image.palette()),
            image_bits: image.bits().to_vec(),
            mask_bits: mask.bits().to_vec(),
        });
    }

    fn directory_header(&self) -> DirectoryHeader {
        DirectoryHeader {
            reserved: 0,
            res_type: RES_CURSOR,
            res_count: self.entries.len() as _,
        }
    }

    fn directory_entries(&self) -> impl Iterator<Item = &CursorDirEntry> {
        self.entries.iter().map(|entry| &entry.dir_entry)
    }

    pub fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        self.directory_header().write_to(&mut writer)?;
        for dir_entry in self.directory_entries() {
            dir_entry.write_to(&mut writer)?;
        }
        for entry in self.entries.iter() {
            entry.bitmap_info.write_to(&mut writer)?;
            writer.write_all(&entry.image_bits)?;
            writer.write_all(&entry.mask_bits)?;
        }
        Ok(())
    }
}

#[derive(Clone, Copy)]
struct DirectoryHeader {
    reserved: u16,
    res_type: u16,
    res_count: u16,
}

impl DirectoryHeader {
    const SIZE: u32 = 6;

    fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        writer.write_le_u16(self.reserved)?;
        writer.write_le_u16(self.res_type)?;
        writer.write_le_u16(self.res_count)?;
        Ok(())
    }
}

const RES_ICON: u16 = 1;
const RES_CURSOR: u16 = 2;

fn get_color_count(bitmap: &impl Bitmap) -> u8 {
    if bitmap.bit_count() <= 8 {
        1 << bitmap.bit_count()
    } else {
        0
    }
}

#[derive(Clone, Copy)]
struct IconDirEntry {
    width: u8,
    height: u8,
    color_count: u8,
    reserved: u8,
    planes: u16,
    bit_count: u16,
    bytes_in_res: u32,
    image_offset: u32,
}

impl IconDirEntry {
    const SIZE: u32 = 16;

    fn new(image: &impl Bitmap, mask: &BitmapOne) -> Self {
        Self {
            width: image.width() as u8,
            height: image.height() as u8,
            color_count: get_color_count(image),
            reserved: 0,
            planes: 1,
            bit_count: image.bit_count(),
            bytes_in_res: image.info().byte_size()
                + image.info_header().size_image
                + mask.info_header().size_image,
            image_offset: 0,
        }
    }

    fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        writer.write_le_u8(self.width)?;
        writer.write_le_u8(self.height)?;
        writer.write_le_u8(self.color_count)?;
        writer.write_le_u8(self.reserved)?;
        writer.write_le_u16(self.planes)?;
        writer.write_le_u16(self.bit_count)?;
        writer.write_le_u32(self.bytes_in_res)?;
        writer.write_le_u32(self.image_offset)?;
        Ok(())
    }
}

#[derive(Clone, Copy)]
struct CursorDirEntry {
    width: u8,
    height: u8,
    color_count: u8,
    reserved: u8,
    x_hot_spot: u16,
    y_hot_spot: u16,
    bytes_in_res: u32,
    image_offset: u32,
}

impl CursorDirEntry {
    const SIZE: u32 = 16;

    fn new(image: &impl Bitmap, mask: &BitmapOne, hotspot: Point) -> Self {
        Self {
            width: image.width() as u8,
            height: image.height() as u8,
            color_count: get_color_count(image),
            reserved: 0,
            x_hot_spot: hotspot.h as u16,
            y_hot_spot: hotspot.v as u16,
            bytes_in_res: image.info().byte_size()
                + image.info_header().size_image
                + mask.info_header().size_image,
            image_offset: 0,
        }
    }

    fn write_to(&self, mut writer: impl Write) -> io::Result<()> {
        writer.write_le_u8(self.width)?;
        writer.write_le_u8(self.height)?;
        writer.write_le_u8(self.color_count)?;
        writer.write_le_u8(self.reserved)?;
        writer.write_le_u16(self.x_hot_spot)?;
        writer.write_le_u16(self.y_hot_spot)?;
        writer.write_le_u32(self.bytes_in_res)?;
        writer.write_le_u32(self.image_offset)?;
        Ok(())
    }
}
