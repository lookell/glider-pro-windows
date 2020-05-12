use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use crate::icocur::IconFile;
use std::io::{self, Read, Write};

struct Icon {
    data: [u8; 128],
}

impl Icon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut data = [0; 128];
        reader.read_exact(&mut data)?;
        Ok(Self { data })
    }
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let icon = Icon::read_from(data)?;

    let mut data_bits = BitmapOne::new(32, 32);
    data_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    super::read_1bit_bitmap_data(&mut data_bits, &icon.data, 0);

    let mut mask_bits = BitmapOne::new(32, 32);
    mask_bits.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
    // bitmaps start out with all zeros, and that's exactly what's needed
    // for an opaque icon mask

    let mut ico_file = IconFile::new();
    ico_file.add_entry(data_bits, mask_bits);
    ico_file.write_to(writer)
}
