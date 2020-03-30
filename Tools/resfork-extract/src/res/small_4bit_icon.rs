use crate::bitmap::{Bitmap, BitmapFour, BitmapOne, RgbQuad};
use crate::icocur::IconFile;
use crate::mac_clut::MAC_COLOR_4;
use crate::rsrcfork::Resource;
use std::io::{self, Read, Write};

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

pub fn get_entry_name(res: &Resource) -> String {
    format!("FinderIcon/{}-small-4bit.ico", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
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

    let mut mask_bits = BitmapOne::new(16, 16);
    mask_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    let mask_bits = mask_bits;
    // bitmap is already all-black, as required for an opaque mask

    let mut ico_file = IconFile::new();
    ico_file.add_entry(data_bits, mask_bits);
    ico_file.write_to(writer)
}
