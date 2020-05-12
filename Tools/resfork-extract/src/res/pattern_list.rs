use crate::bitmap::{Bitmap, BitmapOne, RgbQuad};
use crate::utils::ReadExt;
use std::io::{self, Read};

struct PatternList {
    list: Vec<[u8; 8]>,
}

impl PatternList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let num_patterns = reader.read_be_u16()?;
        let mut list = Vec::with_capacity(num_patterns.into());
        for _ in 0..num_patterns {
            let mut pattern = [0; 8];
            reader.read_exact(&mut pattern)?;
            list.push(pattern);
        }
        Ok(Self { list })
    }
}

pub fn convert(data: &[u8]) -> io::Result<Vec<BitmapOne>> {
    let pattern_list = PatternList::read_from(data)?;
    let mut patterns = Vec::with_capacity(pattern_list.list.len());
    for bits in pattern_list.list {
        let mut patt = BitmapOne::new(8, 8);
        patt.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
        super::read_1bit_bitmap_data(&mut patt, &bits, 0);
        patterns.push(patt);
    }
    Ok(patterns)
}
