use super::{ColorTable, Rect};
use crate::bitmap::{Bitmap, BitmapEight, BitmapFour, BitmapOne, RgbQuad};
use crate::icocur::IconFile;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

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
    let row_bytes = usize::from(bitmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (8 * xbase) as u16;
            output.set_pixel(xbase, y, ((byte & 0x80) == 0).into());
            output.set_pixel(xbase + 1, y, ((byte & 0x40) == 0).into());
            output.set_pixel(xbase + 2, y, ((byte & 0x20) == 0).into());
            output.set_pixel(xbase + 3, y, ((byte & 0x10) == 0).into());
            output.set_pixel(xbase + 4, y, ((byte & 0x08) == 0).into());
            output.set_pixel(xbase + 5, y, ((byte & 0x04) == 0).into());
            output.set_pixel(xbase + 6, y, ((byte & 0x02) == 0).into());
            output.set_pixel(xbase + 7, y, ((byte & 0x01) == 0).into());
        }
    }
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

fn make_4bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapFour {
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (2 * xbase) as u16;
            output.set_pixel(xbase, y, 15 - (byte / 16));
            output.set_pixel(xbase + 1, y, 15 - (byte % 16));
        }
    }
    output
}

fn make_8bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapEight {
    let mut output = BitmapEight::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        for (x, byte) in row.iter().copied().enumerate() {
            output.set_pixel(x as _, y as _, 255 - byte);
        }
    }
    output
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("AppIcon/{}-color.ico", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let icon = ColorIcon::read_from(data)?;
    let mut ico_file = IconFile::new();
    let mask = make_1bit_bitmap(&icon.iconMask, &icon.iconMaskData);
    if !icon.iconBMapData.is_empty() {
        let image = make_1bit_bitmap(&icon.iconBMap, &icon.iconBMapData);
        ico_file.add_entry(image, mask.clone());
    }
    match icon.iconPMap.pixelSize {
        1 => {
            let mut image = make_1bit_pixmap(&icon.iconPMap, &icon.iconPMapData);
            let mut palette = vec![RgbQuad::BLACK; 2];
            palette
                .iter_mut()
                .rev()
                .zip(&icon.iconPMapCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            image.set_palette(palette.into_iter());
            ico_file.add_entry(image, mask);
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
            ico_file.add_entry(image, mask);
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
            ico_file.add_entry(image, mask);
        }
        _ => unimplemented!("unsupported 'cicn' depth: {}", icon.iconPMap.pixelSize),
    };
    ico_file.write_to(writer)
}
