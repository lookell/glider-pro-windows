use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use crate::icocur::IconFile;
use crate::rsrcfork::Resource;
use std::io::{self, Read, Write};

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

pub fn get_entry_name(res: &Resource) -> String {
    format!("FinderIcon/{}-large-mono.ico", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let icon = IconList::read_from(data)?;

    let mut data_bits = BitmapOne::new(32, 32);
    data_bits.set_palette(&[RgbQuad::BLACK, RgbQuad::WHITE]);
    for (y, quad) in icon.data.chunks_exact(4).enumerate() {
        data_bits.set_pixel(0, y as _, ((quad[0] & 0x80) == 0).into());
        data_bits.set_pixel(1, y as _, ((quad[0] & 0x40) == 0).into());
        data_bits.set_pixel(2, y as _, ((quad[0] & 0x20) == 0).into());
        data_bits.set_pixel(3, y as _, ((quad[0] & 0x10) == 0).into());
        data_bits.set_pixel(4, y as _, ((quad[0] & 0x08) == 0).into());
        data_bits.set_pixel(5, y as _, ((quad[0] & 0x04) == 0).into());
        data_bits.set_pixel(6, y as _, ((quad[0] & 0x02) == 0).into());
        data_bits.set_pixel(7, y as _, ((quad[0] & 0x01) == 0).into());

        data_bits.set_pixel(8, y as _, ((quad[1] & 0x80) == 0).into());
        data_bits.set_pixel(9, y as _, ((quad[1] & 0x40) == 0).into());
        data_bits.set_pixel(10, y as _, ((quad[1] & 0x20) == 0).into());
        data_bits.set_pixel(11, y as _, ((quad[1] & 0x10) == 0).into());
        data_bits.set_pixel(12, y as _, ((quad[1] & 0x08) == 0).into());
        data_bits.set_pixel(13, y as _, ((quad[1] & 0x04) == 0).into());
        data_bits.set_pixel(14, y as _, ((quad[1] & 0x02) == 0).into());
        data_bits.set_pixel(15, y as _, ((quad[1] & 0x01) == 0).into());

        data_bits.set_pixel(16, y as _, ((quad[2] & 0x80) == 0).into());
        data_bits.set_pixel(17, y as _, ((quad[2] & 0x40) == 0).into());
        data_bits.set_pixel(18, y as _, ((quad[2] & 0x20) == 0).into());
        data_bits.set_pixel(19, y as _, ((quad[2] & 0x10) == 0).into());
        data_bits.set_pixel(20, y as _, ((quad[2] & 0x08) == 0).into());
        data_bits.set_pixel(21, y as _, ((quad[2] & 0x04) == 0).into());
        data_bits.set_pixel(22, y as _, ((quad[2] & 0x02) == 0).into());
        data_bits.set_pixel(23, y as _, ((quad[2] & 0x01) == 0).into());

        data_bits.set_pixel(24, y as _, ((quad[3] & 0x80) == 0).into());
        data_bits.set_pixel(25, y as _, ((quad[3] & 0x40) == 0).into());
        data_bits.set_pixel(26, y as _, ((quad[3] & 0x20) == 0).into());
        data_bits.set_pixel(27, y as _, ((quad[3] & 0x10) == 0).into());
        data_bits.set_pixel(28, y as _, ((quad[3] & 0x08) == 0).into());
        data_bits.set_pixel(29, y as _, ((quad[3] & 0x04) == 0).into());
        data_bits.set_pixel(30, y as _, ((quad[3] & 0x02) == 0).into());
        data_bits.set_pixel(31, y as _, ((quad[3] & 0x01) == 0).into());
    }
    let data_bits = data_bits;

    let mut mask_bits = BitmapOne::new(32, 32);
    mask_bits.set_palette(&[RgbQuad::BLACK, RgbQuad::WHITE]);
    for (y, quad) in icon.mask.chunks_exact(4).enumerate() {
        mask_bits.set_pixel(0, y as _, ((quad[0] & 0x80) == 0).into());
        mask_bits.set_pixel(1, y as _, ((quad[0] & 0x40) == 0).into());
        mask_bits.set_pixel(2, y as _, ((quad[0] & 0x20) == 0).into());
        mask_bits.set_pixel(3, y as _, ((quad[0] & 0x10) == 0).into());
        mask_bits.set_pixel(4, y as _, ((quad[0] & 0x08) == 0).into());
        mask_bits.set_pixel(5, y as _, ((quad[0] & 0x04) == 0).into());
        mask_bits.set_pixel(6, y as _, ((quad[0] & 0x02) == 0).into());
        mask_bits.set_pixel(7, y as _, ((quad[0] & 0x01) == 0).into());

        mask_bits.set_pixel(8, y as _, ((quad[1] & 0x80) == 0).into());
        mask_bits.set_pixel(9, y as _, ((quad[1] & 0x40) == 0).into());
        mask_bits.set_pixel(10, y as _, ((quad[1] & 0x20) == 0).into());
        mask_bits.set_pixel(11, y as _, ((quad[1] & 0x10) == 0).into());
        mask_bits.set_pixel(12, y as _, ((quad[1] & 0x08) == 0).into());
        mask_bits.set_pixel(13, y as _, ((quad[1] & 0x04) == 0).into());
        mask_bits.set_pixel(14, y as _, ((quad[1] & 0x02) == 0).into());
        mask_bits.set_pixel(15, y as _, ((quad[1] & 0x01) == 0).into());

        mask_bits.set_pixel(16, y as _, ((quad[2] & 0x80) == 0).into());
        mask_bits.set_pixel(17, y as _, ((quad[2] & 0x40) == 0).into());
        mask_bits.set_pixel(18, y as _, ((quad[2] & 0x20) == 0).into());
        mask_bits.set_pixel(19, y as _, ((quad[2] & 0x10) == 0).into());
        mask_bits.set_pixel(20, y as _, ((quad[2] & 0x08) == 0).into());
        mask_bits.set_pixel(21, y as _, ((quad[2] & 0x04) == 0).into());
        mask_bits.set_pixel(22, y as _, ((quad[2] & 0x02) == 0).into());
        mask_bits.set_pixel(23, y as _, ((quad[2] & 0x01) == 0).into());

        mask_bits.set_pixel(24, y as _, ((quad[3] & 0x80) == 0).into());
        mask_bits.set_pixel(25, y as _, ((quad[3] & 0x40) == 0).into());
        mask_bits.set_pixel(26, y as _, ((quad[3] & 0x20) == 0).into());
        mask_bits.set_pixel(27, y as _, ((quad[3] & 0x10) == 0).into());
        mask_bits.set_pixel(28, y as _, ((quad[3] & 0x08) == 0).into());
        mask_bits.set_pixel(29, y as _, ((quad[3] & 0x04) == 0).into());
        mask_bits.set_pixel(30, y as _, ((quad[3] & 0x02) == 0).into());
        mask_bits.set_pixel(31, y as _, ((quad[3] & 0x01) == 0).into());
    }
    let mask_bits = mask_bits;

    let mut ico_file = IconFile::new();
    ico_file.add_entry(data_bits, mask_bits);
    ico_file.write_to(writer)
}
