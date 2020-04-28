use crate::bitmap::{Bitmap, BitmapFour};
use crate::mac_clut::MAC_COLOR_4;
use crate::rsrcfork::Resource;
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

pub fn get_entry_name(res: &Resource) -> String {
    format!("FinderIcon/{}-large-4bit.ico", res.id)
}

pub fn convert(data: &[u8]) -> io::Result<BitmapFour> {
    let icon = Large4BitIcon::read_from(data)?;

    let mut data_bits = BitmapFour::new(32, 32);
    data_bits.set_palette(MAC_COLOR_4.iter().rev().map(|&c| c.into()));
    for (y, row) in icon.data.chunks_exact(16).enumerate() {
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
