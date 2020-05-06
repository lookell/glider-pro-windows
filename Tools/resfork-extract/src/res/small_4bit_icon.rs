use crate::bitmap::{Bitmap, BitmapFour};
use crate::mac_clut::MAC_COLOR_4;
use std::io::{self, Read};

struct Small4BitIcon {
    data: [u8; 128],
}

impl Small4BitIcon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 128];
        reader.read_exact(&mut data)?;
        Ok(Self { data })
    }
}

pub fn convert(data: &[u8]) -> io::Result<BitmapFour> {
    let icon = Small4BitIcon::read_from(data)?;

    let mut data_bits = BitmapFour::new(16, 16);
    data_bits.set_palette(MAC_COLOR_4.iter().rev().map(|&c| c.into()));
    for (y, row) in icon.data.chunks_exact(8).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (2 * xbase) as u16;
            data_bits.set_pixel(xbase, y, 15 - (byte / 16));
            data_bits.set_pixel(xbase + 1, y, 15 - (byte % 16));
        }
    }
    let data_bits = data_bits;

    Ok(data_bits)
}
