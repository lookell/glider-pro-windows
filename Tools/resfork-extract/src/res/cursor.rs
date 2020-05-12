use super::Point;
use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use crate::icocur::CursorFile;
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

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let cursor = Cursor::read_from(data)?;

    let mut data_bits = BitmapOne::new(16, 16);
    data_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_1bit_bitmap_data(&mut data_bits, &cursor.data, 0);

    let mut mask_bits = BitmapOne::new(16, 16);
    mask_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_mask_bitmap_data(&mut mask_bits, &cursor.mask, 0);

    let mut cur_file = CursorFile::new();
    cur_file.add_entry(data_bits, mask_bits, cursor.hotspot);
    cur_file.write_to(writer)
}
