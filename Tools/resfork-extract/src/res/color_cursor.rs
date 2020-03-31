use super::{ColorTable, Point, Rect};
use crate::bitmap::{Bitmap, BitmapFour, BitmapOne, RgbQuad};
use crate::icocur::CursorFile;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Seek, SeekFrom, Write};

struct CCrsr {
    // CCrsr
    crsrType: u16,
    crsrMap: u32,
    crsrData: u32,
    crsrXData: u32,
    crsrXValid: u16,
    crsrXHandle: u32,
    crsr1Data: [u8; 32],
    crsrMask: [u8; 32],
    crsrHotSpot: Point,
    crsrXTable: u32,
    crsrID: u32,
    // PixMap
    baseAddr: u32,
    rowBytes: u16,
    bounds: Rect,
    pmVersion: u16,
    packType: u16,
    packSize: u32,
    hRes: i32,
    vRes: i32,
    pixelType: u16,
    pixelSize: u16,
    cmpCount: u16,
    cmpSize: u16,
    planeBytes: u32,
    pmTable: u32,
    pmReserved: u32,
    // pixel data
    pixelData: Vec<u8>,
    // color table data
    pixelCTab: ColorTable,
}

impl CCrsr {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Self> {
        let crsrType = reader.read_be_u16()?;
        let crsrMap = reader.read_be_u32()?;
        let crsrData = reader.read_be_u32()?;
        let crsrXData = reader.read_be_u32()?;
        let crsrXValid = reader.read_be_u16()?;
        let crsrXHandle = reader.read_be_u32()?;
        let mut crsr1Data = [0; 32];
        reader.read_exact(&mut crsr1Data)?;
        let mut crsrMask = [0; 32];
        reader.read_exact(&mut crsrMask)?;
        let crsrHotSpot = Point::read_from(&mut reader)?;
        let crsrXTable = reader.read_be_u32()?;
        let crsrID = reader.read_be_u32()?;

        reader.seek(SeekFrom::Start(crsrMap as _))?;
        let baseAddr = reader.read_be_u32()?;
        let rowBytes = reader.read_be_u16()?;
        let bounds = Rect::read_from(&mut reader)?;
        let pmVersion = reader.read_be_u16()?;
        let packType = reader.read_be_u16()?;
        let packSize = reader.read_be_u32()?;
        let hRes = reader.read_be_i32()?;
        let vRes = reader.read_be_i32()?;
        let pixelType = reader.read_be_u16()?;
        let pixelSize = reader.read_be_u16()?;
        let cmpCount = reader.read_be_u16()?;
        let cmpSize = reader.read_be_u16()?;
        let planeBytes = reader.read_be_u32()?;
        let pmTable = reader.read_be_u32()?;
        let pmReserved = reader.read_be_u32()?;

        reader.seek(SeekFrom::Start(crsrData as _))?;
        let mut pixelData = vec![0x00; pmTable.saturating_sub(crsrData) as _];
        reader.read_exact(&mut pixelData)?;

        reader.seek(SeekFrom::Start(pmTable as _))?;
        let pixelCTab = ColorTable::read_from(&mut reader)?;

        Ok(Self {
            crsrType,
            crsrMap,
            crsrData,
            crsrXData,
            crsrXValid,
            crsrXHandle,
            crsr1Data,
            crsrMask,
            crsrHotSpot,
            crsrXTable,
            crsrID,
            baseAddr,
            rowBytes,
            bounds,
            pmVersion,
            packType,
            packSize,
            hRes,
            vRes,
            pixelType,
            pixelSize,
            cmpCount,
            cmpSize,
            planeBytes,
            pmTable,
            pmReserved,
            pixelData,
            pixelCTab,
        })
    }
}

fn convert_mask_data(cursor: &CCrsr) -> BitmapOne {
    let mut mask_bits = BitmapOne::new(16, 16);
    for (y, row) in cursor.crsrMask.chunks_exact(2).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (8 * xbase) as u16;
            mask_bits.set_pixel(xbase, y, ((byte & 0x80) == 0).into());
            mask_bits.set_pixel(xbase + 1, y, ((byte & 0x40) == 0).into());
            mask_bits.set_pixel(xbase + 2, y, ((byte & 0x20) == 0).into());
            mask_bits.set_pixel(xbase + 3, y, ((byte & 0x10) == 0).into());
            mask_bits.set_pixel(xbase + 4, y, ((byte & 0x08) == 0).into());
            mask_bits.set_pixel(xbase + 5, y, ((byte & 0x04) == 0).into());
            mask_bits.set_pixel(xbase + 6, y, ((byte & 0x02) == 0).into());
            mask_bits.set_pixel(xbase + 7, y, ((byte & 0x01) == 0).into());
        }
    }
    mask_bits
}

fn convert_1bit_data(pixel_data: &[u8], bitmap: &mut BitmapOne) {
    for (y, row) in pixel_data.chunks_exact(2).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (8 * xbase) as u16;
            bitmap.set_pixel(xbase, y, ((byte & 0x80) == 0).into());
            bitmap.set_pixel(xbase + 1, y, ((byte & 0x40) == 0).into());
            bitmap.set_pixel(xbase + 2, y, ((byte & 0x20) == 0).into());
            bitmap.set_pixel(xbase + 3, y, ((byte & 0x10) == 0).into());
            bitmap.set_pixel(xbase + 4, y, ((byte & 0x08) == 0).into());
            bitmap.set_pixel(xbase + 5, y, ((byte & 0x04) == 0).into());
            bitmap.set_pixel(xbase + 6, y, ((byte & 0x02) == 0).into());
            bitmap.set_pixel(xbase + 7, y, ((byte & 0x01) == 0).into());
        }
    }
}

fn convert_2bit_data(pixel_data: &[u8], bitmap: &mut BitmapFour) {
    for (y, row) in pixel_data.chunks_exact(4).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (4 * xbase) as u16;
            bitmap.set_pixel(xbase, y, 15 - (byte / 64));
            bitmap.set_pixel(xbase + 1, y, 15 - (byte / 16 % 4));
            bitmap.set_pixel(xbase + 2, y, 15 - (byte / 4 % 4));
            bitmap.set_pixel(xbase + 3, y, 15 - (byte % 4));
        }
    }
}

fn convert_4bit_data(pixel_data: &[u8], bitmap: &mut BitmapFour) {
    for (y, row) in pixel_data.chunks_exact(8).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (2 * xbase) as u16;
            bitmap.set_pixel(xbase, y, 15 - (byte / 16));
            bitmap.set_pixel(xbase + 1, y, 15 - (byte % 16));
        }
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Cursor/{}-color.cur", res.id)
}

// HACK: In theory, the color table could contain any arrangement
// of colors. In practise, however, Macintosh color tables have
// a white entry at the first index and a black entry at the last
// index. To get black to be index 0, each pixel index is subtracted
// from the maximum value and the color table is reversed. This is
// done to meet Windows .ico conventions, where black is the first
// entry in the color palette.

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let cursor = CCrsr::read_from(io::Cursor::new(data))?;
    let mut cur_file = CursorFile::new();
    let cursor_mask = convert_mask_data(&cursor);

    match cursor.pixelSize {
        2 => {
            let mut palette = vec![RgbQuad::BLACK; 16];
            palette
                .iter_mut()
                .rev()
                .zip(&cursor.pixelCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            let mut bitmap = BitmapFour::new(16, 16);
            bitmap.set_palette(palette.iter().copied());
            convert_2bit_data(&cursor.pixelData, &mut bitmap);
            cur_file.add_entry(bitmap, cursor_mask.clone(), cursor.crsrHotSpot);
        }
        4 => {
            let mut palette = vec![RgbQuad::BLACK; 16];
            palette
                .iter_mut()
                .rev()
                .zip(&cursor.pixelCTab.ctTable)
                .for_each(|(dst, src)| *dst = src.rgb.into());
            let mut bitmap = BitmapFour::new(16, 16);
            bitmap.set_palette(palette.iter().copied());
            convert_4bit_data(&cursor.pixelData, &mut bitmap);
            cur_file.add_entry(bitmap, cursor_mask.clone(), cursor.crsrHotSpot);
        }
        _ => panic!("'crsr': unsupported color depth ({})", cursor.cmpSize),
    }

    let mut mono_bits = BitmapOne::new(16, 16);
    mono_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    convert_1bit_data(&cursor.crsr1Data, &mut mono_bits);
    cur_file.add_entry(mono_bits, cursor_mask, cursor.crsrHotSpot);

    cur_file.write_to(writer)
}
