use super::Point;
use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use crate::icocur::CursorFile;
use crate::rsrcfork::Resource;
use std::io::{self, Read, Write};

struct Cursor {
    data: [u8; 32],
    mask: [u8; 32],
    hotspot: Point,
}

impl Cursor {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 32];
        reader.read_exact(&mut data)?;
        let mut mask = [0; 32];
        reader.read_exact(&mut mask)?;
        let hotspot = Point::read_from(&mut reader)?;
        Ok(Self {
            data,
            mask,
            hotspot,
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Cursor/{}-mono.cur", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let cursor = Cursor::read_from(data)?;

    let mut data_bits = BitmapOne::new(16, 16);
    data_bits.set_palette(&[RgbQuad::BLACK, RgbQuad::WHITE]);
    for (y, pair) in cursor.data.chunks_exact(2).enumerate() {
        let y = y as u16;
        for (xbase, byte) in pair.iter().copied().enumerate() {
            let xbase = (8 * xbase) as u16;
            data_bits.set_pixel(xbase, y, ((byte & 0x80) == 0).into());
            data_bits.set_pixel(xbase + 1, y, ((byte & 0x40) == 0).into());
            data_bits.set_pixel(xbase + 2, y, ((byte & 0x20) == 0).into());
            data_bits.set_pixel(xbase + 3, y, ((byte & 0x10) == 0).into());
            data_bits.set_pixel(xbase + 4, y, ((byte & 0x08) == 0).into());
            data_bits.set_pixel(xbase + 5, y, ((byte & 0x04) == 0).into());
            data_bits.set_pixel(xbase + 6, y, ((byte & 0x02) == 0).into());
            data_bits.set_pixel(xbase + 7, y, ((byte & 0x01) == 0).into());
        }
    }
    let data_bits = data_bits;

    let mut mask_bits = BitmapOne::new(16, 16);
    mask_bits.set_palette(&[RgbQuad::BLACK, RgbQuad::WHITE]);
    for (y, pair) in cursor.mask.chunks_exact(2).enumerate() {
        let y = y as u16;
        for (xbase, byte) in pair.iter().copied().enumerate() {
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
    let mask_bits = mask_bits;

    let mut cur_file = CursorFile::new();
    cur_file.add_entry(data_bits, mask_bits, cursor.hotspot);
    cur_file.write_to(writer)
}
