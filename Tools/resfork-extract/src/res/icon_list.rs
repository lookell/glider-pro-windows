use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use std::io::{self, Read};

struct IconList {
    data: [u8; 128],
    mask: [u8; 128],
}

impl IconList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 128];
        reader.read_exact(&mut data)?;
        let mut mask = [0; 128];
        reader.read_exact(&mut mask)?;
        Ok(Self { data, mask })
    }
}

pub fn convert(data: &[u8]) -> io::Result<(BitmapOne, BitmapOne)> {
    let icon = IconList::read_from(data)?;

    let mut data_bits = BitmapOne::new(32, 32);
    data_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    for (y, quad) in icon.data.chunks_exact(4).enumerate() {
        let y = y as u16;
        for (xbase, byte) in quad.iter().copied().enumerate() {
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

    let mut mask_bits = BitmapOne::new(32, 32);
    mask_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    for (y, quad) in icon.mask.chunks_exact(4).enumerate() {
        let y = y as u16;
        for (xbase, byte) in quad.iter().copied().enumerate() {
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

    Ok((data_bits, mask_bits))
}
