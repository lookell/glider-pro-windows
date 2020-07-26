use super::{ColorTable, Rect};
use crate::bitmap::{Bitmap, BitmapEight, BitmapFour, BitmapOne, RgbQuad};
use crate::icocur::IconFile;
use crate::utils::ReadExt;
use std::io::{self, ErrorKind, Read, Write};

struct PixMap {
    baseAddr: u32,
    rowBytes: u16,
    bounds: Rect,
    pmVersion: u16,
    packType: u16,
    packSize: u32,
    hRes: u32,
    vRes: u32,
    pixelType: u16,
    pixelSize: u16,
    cmpCount: u16,
    cmpSize: u16,
    planeBytes: u32,
    pmTable: u32,
    pmReserved: u32,
}

impl PixMap {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            baseAddr: reader.read_be_u32()?,
            rowBytes: reader.read_be_u16()?,
            bounds: Rect::read_from(&mut reader)?,
            pmVersion: reader.read_be_u16()?,
            packType: reader.read_be_u16()?,
            packSize: reader.read_be_u32()?,
            hRes: reader.read_be_u32()?,
            vRes: reader.read_be_u32()?,
            pixelType: reader.read_be_u16()?,
            pixelSize: reader.read_be_u16()?,
            cmpCount: reader.read_be_u16()?,
            cmpSize: reader.read_be_u16()?,
            planeBytes: reader.read_be_u32()?,
            pmTable: reader.read_be_u32()?,
            pmReserved: reader.read_be_u32()?,
        })
    }

    fn width(&self) -> u16 {
        (self.bounds.right - self.bounds.left) as _
    }

    fn height(&self) -> u16 {
        (self.bounds.bottom - self.bounds.top) as _
    }

    fn row_bytes(&self) -> u16 {
        self.rowBytes & 0x3FFF
    }
}

struct BitMap {
    baseAddr: u32,
    rowBytes: u16,
    bounds: Rect,
}

impl BitMap {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            baseAddr: reader.read_be_u32()?,
            rowBytes: reader.read_be_u16()?,
            bounds: Rect::read_from(&mut reader)?,
        })
    }

    fn width(&self) -> u16 {
        (self.bounds.right - self.bounds.left) as _
    }

    fn height(&self) -> u16 {
        (self.bounds.bottom - self.bounds.top) as _
    }

    fn row_bytes(&self) -> u16 {
        self.rowBytes & 0x3FFF
    }
}

struct ColorIcon {
    iconPMap: PixMap,
    iconMask: BitMap,
    iconBMap: BitMap,
    iconData: u32,
    iconMaskData: Vec<u8>,
    iconBMapData: Vec<u8>,
    iconPMapCTab: ColorTable,
    iconPMapData: Vec<u8>,
}

impl ColorIcon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let iconPMap = PixMap::read_from(&mut reader).unwrap();
        let iconMask = BitMap::read_from(&mut reader).unwrap();
        let iconBMap = BitMap::read_from(&mut reader).unwrap();
        let iconData = reader.read_be_u32()?;
        let height = usize::from(iconPMap.height());
        let mut iconMaskData = vec![0x00; usize::from(iconMask.row_bytes()) * height];
        reader.read_exact(&mut iconMaskData).unwrap();
        let mut iconBMapData = vec![0x00; usize::from(iconBMap.row_bytes()) * height];
        reader.read_exact(&mut iconBMapData).unwrap();
        let iconPMapCTab = ColorTable::read_from(&mut reader).unwrap();
        let mut iconPMapData = vec![0x00; usize::from(iconPMap.row_bytes()) * height];
        reader.read_exact(&mut iconPMapData).unwrap();
        Ok(Self {
            iconPMap,
            iconMask,
            iconBMap,
            iconData,
            iconMaskData,
            iconBMapData,
            iconPMapCTab,
            iconPMapData,
        })
    }

    fn width(&self) -> u16 {
        self.iconPMap.width()
    }

    fn height(&self) -> u16 {
        self.iconPMap.height()
    }
}

fn make_1bit_bitmap(bitmap: &BitMap, bits: &[u8]) -> BitmapOne {
    let mut output = BitmapOne::new(bitmap.width(), bitmap.height());
    output.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_1bit_bitmap_data(&mut output, bits, bitmap.row_bytes());
    output
}

fn make_1bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapOne {
    let bitmap = BitMap {
        baseAddr: pixmap.baseAddr,
        rowBytes: pixmap.rowBytes,
        bounds: pixmap.bounds,
    };
    make_1bit_bitmap(&bitmap, bits)
}

fn make_2bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapFour {
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    super::read_2bit_bitmap_data(&mut output, bits, pixmap.row_bytes());
    output
}

fn make_4bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapFour {
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    super::read_4bit_bitmap_data(&mut output, bits, pixmap.row_bytes());
    output
}

fn make_8bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapEight {
    let mut output = BitmapEight::new(pixmap.width(), pixmap.height());
    super::read_8bit_bitmap_data(&mut output, bits, pixmap.row_bytes());
    output
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let icon = ColorIcon::read_from(data)?;
    let mut ico_file = IconFile::new();
    let mask = make_1bit_bitmap(&icon.iconMask, &icon.iconMaskData);
    match icon.iconPMap.pixelSize {
        1 => {
            let mono_image = make_1bit_pixmap(&icon.iconPMap, &icon.iconPMapData);
            let mut image = BitmapFour::from(mono_image);
            let mut palette = vec![RgbQuad::BLACK; 16];
            palette
                .iter_mut()
                .rev()
                .zip(&icon.iconPMapCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            image.set_palette(palette.into_iter());
            for y in 0..image.height() {
                for x in 0..image.width() {
                    let old = image.get_pixel(x, y);
                    image.set_pixel(x, y, old + 14);
                }
            }
            ico_file.add_entry(image, mask.clone());
        }
        2 => {
            let mut image = make_2bit_pixmap(&icon.iconPMap, &icon.iconPMapData);
            let mut palette = vec![RgbQuad::BLACK; 16];
            palette
                .iter_mut()
                .rev()
                .zip(&icon.iconPMapCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            image.set_palette(palette.into_iter());
            ico_file.add_entry(image, mask.clone());
        }
        4 => {
            let mut image = make_4bit_pixmap(&icon.iconPMap, &icon.iconPMapData);
            let mut palette = vec![RgbQuad::BLACK; 16];
            palette
                .iter_mut()
                .rev()
                .zip(&icon.iconPMapCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            image.set_palette(palette.into_iter());
            ico_file.add_entry(image, mask.clone());
        }
        8 => {
            let mut image = make_8bit_pixmap(&icon.iconPMap, &icon.iconPMapData);
            let mut palette = vec![RgbQuad::BLACK; 256];
            palette
                .iter_mut()
                .rev()
                .zip(&icon.iconPMapCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            image.set_palette(palette.into_iter());
            ico_file.add_entry(image, mask.clone());
        }
        _ => {
            return Err(io::Error::new(
                ErrorKind::InvalidData,
                format!("unsupported 'cicn' depth: {}", icon.iconPMap.pixelSize),
            ));
        }
    };
    if !icon.iconBMapData.is_empty() {
        let image = make_1bit_bitmap(&icon.iconBMap, &icon.iconBMapData);
        ico_file.add_entry(image, mask.clone());
    }
    ico_file.write_to(writer)
}
