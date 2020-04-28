use crate::bitmap::{Bitmap, BitmapEight};
use crate::mac_clut::MAC_COLOR_8;
use crate::rsrcfork::Resource;
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

pub fn get_entry_name(res: &Resource) -> String {
    format!("FinderIcon/{}-large-8bit.ico", res.id)
}

pub fn convert(data: &[u8]) -> io::Result<BitmapEight> {
    let icon = Large8BitIcon::read_from(data)?;

    let mut data_bits = BitmapEight::new(32, 32);
    data_bits.set_palette(MAC_COLOR_8.iter().rev().map(|&c| c.into()));
    for (y, row) in icon.data.chunks_exact(32).enumerate() {
        for (x, byte) in row.iter().copied().enumerate() {
            data_bits.set_pixel(x as _, y as _, 255 - byte);
        }
    }
    let data_bits = data_bits;

    Ok(data_bits)
}
