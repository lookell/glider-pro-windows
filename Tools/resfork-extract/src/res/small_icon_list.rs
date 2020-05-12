use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use std::io::{self, Read};

struct SmallIconList {
    data: [u8; 32],
    mask: [u8; 32],
}

impl SmallIconList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 32];
        reader.read_exact(&mut data)?;
        let mut mask = [0; 32];
        reader.read_exact(&mut mask)?;
        Ok(Self { data, mask })
    }
}

pub fn convert(data: &[u8]) -> io::Result<(BitmapOne, BitmapOne)> {
    let icon = SmallIconList::read_from(data)?;

    let mut data_bits = BitmapOne::new(16, 16);
    data_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_1bit_bitmap_data(&mut data_bits, &icon.data, 0);

    let mut mask_bits = BitmapOne::new(16, 16);
    mask_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_mask_bitmap_data(&mut mask_bits, &icon.mask, 0);

    Ok((data_bits, mask_bits))
}
