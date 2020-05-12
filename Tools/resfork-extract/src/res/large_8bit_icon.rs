use crate::bitmap::{Bitmap, BitmapEight};
use crate::mac_clut::MAC_COLOR_8;
use std::io::{self, Read};

struct Large8BitIcon {
    data: [u8; 1024],
}

impl Large8BitIcon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 1024];
        reader.read_exact(&mut data)?;
        Ok(Self { data })
    }
}

pub fn convert(data: &[u8]) -> io::Result<BitmapEight> {
    let icon = Large8BitIcon::read_from(data)?;

    let mut data_bits = BitmapEight::new(32, 32);
    data_bits.set_palette(MAC_COLOR_8.iter().rev().map(|&c| c.into()));
    super::read_8bit_bitmap_data(&mut data_bits, &icon.data, 0);

    Ok(data_bits)
}
