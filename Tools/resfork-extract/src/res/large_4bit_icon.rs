use crate::bitmap::{Bitmap, BitmapFour};
use crate::mac_clut::MAC_COLOR_4;
use std::io::{self, Read};

struct Large4BitIcon {
    data: [u8; 512],
}

impl Large4BitIcon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 512];
        reader.read_exact(&mut data)?;
        Ok(Self { data })
    }
}

pub fn convert(data: &[u8]) -> io::Result<BitmapFour> {
    let icon = Large4BitIcon::read_from(data)?;

    let mut data_bits = BitmapFour::new(32, 32);
    data_bits.set_palette(MAC_COLOR_4.iter().rev().map(|&c| c.into()));
    super::read_4bit_bitmap_data(&mut data_bits, &icon.data, 0);

    Ok(data_bits)
}
