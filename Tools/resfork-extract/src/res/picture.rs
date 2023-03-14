use super::{ColorTable, Point, Rect};
use crate::bitmap::{
    Bitmap, BitmapEight, BitmapFour, BitmapOne, BitmapSixteen, BitmapTwentyFour, RgbQuad,
};
use crate::rsrcfork::Resource;
use crate::utils::{ReadExt, SeekExt};

use std::collections::HashSet;
use std::io::{self, ErrorKind, Read, Seek, SeekFrom, Write};

#[derive(Debug)]
struct Region {
    rgnSize: u16,
    rgnBBox: Rect,
    rgnData: Vec<u8>,
}

impl Region {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let rgnSize = reader.read_be_u16()?;
        let rgnBBox = Rect::read_from(&mut reader)?;
        let rgnData = read_bytes(reader, (rgnSize - 10).into())?;
        Ok(Self {
            rgnSize,
            rgnBBox,
            rgnData,
        })
    }
}

struct Polygon {
    polySize: u16,
    polyBBox: Rect,
    polyData: Vec<u8>,
}

impl Polygon {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let polySize = reader.read_be_u16()?;
        let polyBBox = Rect::read_from(&mut reader)?;
        let polyData = read_bytes(reader, (polySize - 10).into())?;
        Ok(Self {
            polySize,
            polyBBox,
            polyData,
        })
    }
}

#[derive(Clone, Copy, Default)]
struct PixMap {
    baseAddr: u32,
    rowBytes: u16,
    bounds: Rect,
    pmVersion: u16,
    packType: u16,
    packSize: u32,
    hRes: u32,
    vRes: u32,
    pixelType: u16,
    pixelSize: u16,
    cmpCount: u16,
    cmpSize: u16,
    planeBytes: u32,
    pmTable: u32,
    pmReserved: u32,
}

impl PixMap {
    fn read_full(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            baseAddr: reader.read_be_u32()?,
            rowBytes: reader.read_be_u16()?,
            bounds: Rect::read_from(&mut reader)?,
            pmVersion: reader.read_be_u16()?,
            packType: reader.read_be_u16()?,
            packSize: reader.read_be_u32()?,
            hRes: reader.read_be_u32()?,
            vRes: reader.read_be_u32()?,
            pixelType: reader.read_be_u16()?,
            pixelSize: reader.read_be_u16()?,
            cmpCount: reader.read_be_u16()?,
            cmpSize: reader.read_be_u16()?,
            planeBytes: reader.read_be_u32()?,
            pmTable: reader.read_be_u32()?,
            pmReserved: reader.read_be_u32()?,
        })
    }

    fn read_partial(mut reader: impl Read) -> io::Result<Self> {
        let this = Self {
            rowBytes: reader.read_be_u16()?,
            bounds: Rect::read_from(&mut reader)?,
            ..Self::default()
        };
        if this.rowBytes & 0x8000 != 0 {
            Ok(Self {
                pmVersion: reader.read_be_u16()?,
                packType: reader.read_be_u16()?,
                packSize: reader.read_be_u32()?,
                hRes: reader.read_be_u32()?,
                vRes: reader.read_be_u32()?,
                pixelType: reader.read_be_u16()?,
                pixelSize: reader.read_be_u16()?,
                cmpCount: reader.read_be_u16()?,
                cmpSize: reader.read_be_u16()?,
                planeBytes: reader.read_be_u32()?,
                pmTable: reader.read_be_u32()?,
                pmReserved: reader.read_be_u32()?,
                ..this
            })
        } else {
            Ok(Self {
                pixelSize: 1,
                cmpCount: 1,
                cmpSize: 1,
                ..this
            })
        }
    }

    fn width(&self) -> u16 {
        (self.bounds.right - self.bounds.left) as _
    }

    fn height(&self) -> u16 {
        (self.bounds.bottom - self.bounds.top) as _
    }

    fn row_bytes(&self) -> u16 {
        self.rowBytes & 0x1FFF
    }
}

#[derive(Clone)]
struct BitsRect {
    pixMap: PixMap,
    ctTable: ColorTable,
    srcRect: Rect,
    dstRect: Rect,
    mode: i16,
    data: PixData,
}

impl BitsRect {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let pixMap = PixMap::read_partial(&mut reader)?;
        let ctTable = if pixMap.rowBytes & 0x8000 != 0 {
            ColorTable::read_from(&mut reader)?
        } else {
            ColorTable::default()
        };
        Ok(Self {
            pixMap,
            ctTable,
            srcRect: Rect::read_from(&mut reader)?,
            dstRect: Rect::read_from(&mut reader)?,
            mode: reader.read_be_i16()?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }

    // Return whether the two BitsRect are incompatible with each other,
    // in terms of color table and pixel bit depth.
    fn is_incompatible_with(&self, other: &Self) -> bool {
        if self.pixMap.pixelSize != other.pixMap.pixelSize {
            // incompatible because the pixel bit depths don't match
            return true;
        }
        let rgb_iter_1 = self.ctTable.ctTable.iter().map(|elem| elem.rgb);
        let rgb_iter_2 = other.ctTable.ctTable.iter().map(|elem| elem.rgb);
        if rgb_iter_1.ne(rgb_iter_2) {
            // incompatible because the color table values don't match
            return true;
        }
        false
    }
}

struct BitsRgn {
    pixMap: PixMap,
    ctTable: ColorTable,
    srcRect: Rect,
    dstRect: Rect,
    mode: i16,
    maskRgn: Region,
    data: PixData,
}

impl BitsRgn {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let pixMap = PixMap::read_partial(&mut reader)?;
        let ctTable = if pixMap.rowBytes & 0x8000 != 0 {
            ColorTable::read_from(&mut reader)?
        } else {
            ColorTable::default()
        };
        Ok(Self {
            pixMap,
            ctTable,
            srcRect: Rect::read_from(&mut reader)?,
            dstRect: Rect::read_from(&mut reader)?,
            mode: reader.read_be_i16()?,
            maskRgn: Region::read_from(&mut reader)?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

struct DirectBitsRect {
    pixMap: PixMap,
    srcRect: Rect,
    dstRect: Rect,
    mode: i16,
    data: PixData,
}

impl DirectBitsRect {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let pixMap = PixMap::read_full(&mut reader)?;
        Ok(Self {
            pixMap,
            srcRect: Rect::read_from(&mut reader)?,
            dstRect: Rect::read_from(&mut reader)?,
            mode: reader.read_be_i16()?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

struct DirectBitsRgn {
    pixMap: PixMap,
    srcRect: Rect,
    dstRect: Rect,
    mode: i16,
    maskRgn: Region,
    data: PixData,
}

impl DirectBitsRgn {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let pixMap = PixMap::read_full(&mut reader)?;
        Ok(Self {
            pixMap,
            srcRect: Rect::read_from(&mut reader)?,
            dstRect: Rect::read_from(&mut reader)?,
            mode: reader.read_be_i16()?,
            maskRgn: Region::read_from(&mut reader)?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

#[derive(Clone)]
struct PixData {
    data: Vec<u8>,
}

impl PixData {
    fn indexed_read_from<R: Read>(mut reader: R, pixMap: &PixMap) -> io::Result<Self> {
        let num_scanlines = usize::from(pixMap.height());
        if pixMap.row_bytes() < 8 {
            let size = usize::from(pixMap.row_bytes()) * num_scanlines;
            let data = read_bytes(reader, size)?;
            Ok(Self { data })
        } else {
            let mut data = Vec::new();
            for _ in 0..num_scanlines {
                let byteCount = if pixMap.row_bytes() > 250 {
                    reader.read_be_u16()?
                } else {
                    u16::from(reader.read_be_u8()?)
                };
                let packed_data = read_bytes(&mut reader, byteCount.into())?;
                let scanline = unpack_bits(&packed_data)
                    .ok_or_else(|| io::Error::new(ErrorKind::InvalidData, "UnpackBits failed"))?;
                data.extend_from_slice(&scanline);
            }
            Ok(Self { data })
        }
    }

    fn direct_read_from(mut reader: impl Read, pixMap: &PixMap) -> io::Result<Self> {
        let num_scanlines = usize::from(pixMap.height());
        let packType = match (pixMap.packType, pixMap.pixelSize) {
            (0, 16) => 3,
            (0, 32) => 4,
            (1, 16) => pixMap.packType,
            (1, 32) => pixMap.packType,
            (2, 32) => pixMap.packType,
            (3, 16) => pixMap.packType,
            (4, 32) => pixMap.packType,
            _ => {
                return Err(io::Error::new(
                    io::ErrorKind::InvalidData,
                    format!(
                        "unknown (packType, pixelSize) combo: ({}, {})",
                        pixMap.packType, pixMap.pixelSize
                    ),
                ))
            }
        };
        if packType == 1 || pixMap.row_bytes() < 8 {
            let size = usize::from(pixMap.row_bytes()) * num_scanlines;
            let data = read_bytes(&mut reader, size)?;
            Ok(Self { data })
        } else if packType == 2 {
            let size = usize::from(pixMap.row_bytes()) * num_scanlines * 3 / 4;
            let packed_data = read_bytes(&mut reader, size)?;
            let mut data = Vec::with_capacity(size * 4 / 3);
            for chunk in packed_data.chunks_exact(3) {
                data.extend_from_slice(&[0x00, chunk[0], chunk[1], chunk[2]]);
            }
            Ok(Self { data })
        } else if packType == 3 {
            let mut data = Vec::new();
            for _ in 0..num_scanlines {
                let byteCount = if pixMap.row_bytes() > 250 {
                    reader.read_be_u16()?
                } else {
                    u16::from(reader.read_be_u8()?)
                };
                let packed_data = read_bytes(&mut reader, byteCount.into())?;
                let scanline = unpack_words(&packed_data)
                    .ok_or_else(|| io::Error::new(ErrorKind::InvalidData, "UnpackWords failed"))?;
                data.extend_from_slice(&scanline);
            }
            Ok(Self { data })
        } else if packType == 4 {
            let mut data = Vec::new();
            for _ in 0..num_scanlines {
                let byteCount = if pixMap.row_bytes() > 250 {
                    reader.read_be_u16()?
                } else {
                    u16::from(reader.read_be_u8()?)
                };
                let packed_data = read_bytes(&mut reader, byteCount.into())?;
                let scanline = unpack_bits(&packed_data)
                    .ok_or_else(|| io::Error::new(ErrorKind::InvalidData, "UnpackBits failed"))?;
                let scanline_slice = match pixMap.cmpCount {
                    3 => &scanline[..],
                    4 => {
                        // The pad bytes in the 32-bit pixels was included in the
                        // data, so ignore the first quarter of the data. This is
                        // where the pad bytes are located.
                        &scanline[(scanline.len() / 4)..]
                    }
                    _ => {
                        return Err(io::Error::new(
                            ErrorKind::InvalidData,
                            "invalid component count for 32-bit packed image",
                        ));
                    }
                };
                let third_of_len = scanline_slice.len() / 3;
                let rest = scanline_slice;
                let (red_scanline, rest) = rest.split_at(third_of_len);
                let (green_scanline, rest) = rest.split_at(third_of_len);
                let (blue_scanline, _) = rest.split_at(third_of_len);
                for idx in 0..third_of_len {
                    data.extend_from_slice(&[
                        0x00,
                        red_scanline[idx],
                        green_scanline[idx],
                        blue_scanline[idx],
                    ]);
                }
            }
            Ok(Self { data })
        } else {
            return Err(io::Error::new(
                ErrorKind::InvalidData,
                "PixData::direct_read_from",
            ));
        }
    }

    fn read_from(reader: impl Read, pixMap: &PixMap) -> io::Result<Self> {
        if pixMap.pixelType == 0 {
            Self::indexed_read_from(reader, pixMap)
        } else {
            Self::direct_read_from(reader, pixMap)
        }
    }
}

fn unpack_bits(packed: &[u8]) -> Option<Vec<u8>> {
    let mut unpacked = Vec::with_capacity(packed.len());
    let mut source = packed.iter().copied();
    while let Some(header) = source.next() {
        let header = header as i8;
        match header {
            // (-128) is a no-op, so continue to the next header byte.
            -128 => continue,
            // 0 means the next byte is written verbatim.
            0 => unpacked.push(source.next()?),
            // A positive header byte `n` means the next `n + 1` bytes are
            // written verbatim.
            1..=127 => {
                let num_bytes = header as u8 + 1;
                for _ in 0..num_bytes {
                    unpacked.push(source.next()?);
                }
            }
            // A negative header byte `n` means the next byte is repeated
            // `(-n) + 1` times.
            -127..=-1 => {
                let num_bytes = (-header) as u8 + 1;
                let next = source.next()?;
                for _ in 0..num_bytes {
                    unpacked.push(next);
                }
            }
        }
    }
    unpacked.shrink_to_fit();
    Some(unpacked)
}

fn unpack_words(packed: &[u8]) -> Option<Vec<u8>> {
    let mut unpacked = Vec::with_capacity(packed.len());
    let mut source = packed.iter().copied();
    while let Some(header) = source.next() {
        let header = header as i8;
        match header {
            // (-128) is a no-op, so continue to the next header byte.
            -128 => continue,
            // 0 means the next word is written verbatim.
            0 => {
                unpacked.push(source.next()?);
                unpacked.push(source.next()?);
            }
            // A positive header byte `n` means the next `n + 1` words are
            // written verbatim.
            1..=127 => {
                let num_words = header as u8 + 1;
                for _ in 0..num_words {
                    unpacked.push(source.next()?);
                    unpacked.push(source.next()?);
                }
            }
            // A negative header byte `n` means the next word is repeated
            // `(-n) + 1` times.
            -127..=-1 => {
                let num_words = (-header) as u8 + 1;
                let next_1 = source.next()?;
                let next_2 = source.next()?;
                for _ in 0..num_words {
                    unpacked.push(next_1);
                    unpacked.push(next_2);
                }
            }
        }
    }
    unpacked.shrink_to_fit();
    Some(unpacked)
}

fn get_pict_palette(bits_rect: &BitsRect) -> Vec<RgbQuad> {
    let depth = bits_rect.pixMap.pixelSize;
    let mut palette = match depth {
        1 => vec![RgbQuad::BLACK; 2],
        2 | 4 => vec![RgbQuad::BLACK; 16],
        8 => vec![RgbQuad::BLACK; 256],
        _ => vec![],
    };
    let clut = &bits_rect.ctTable.ctTable;
    for (dst, src) in palette.iter_mut().rev().zip(clut) {
        *dst = src.rgb.into();
    }
    if clut.is_empty() && depth == 1 {
        palette = vec![RgbQuad::BLACK, RgbQuad::WHITE];
    }
    palette
}

fn make_1bit_pixmap(bits_rect: &BitsRect) -> BitmapOne {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapOne::new(pixmap.width(), pixmap.height());
    output.set_palette(get_pict_palette(bits_rect));
    super::read_1bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

fn make_2bit_pixmap(bits_rect: &BitsRect) -> BitmapFour {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    output.set_palette(get_pict_palette(bits_rect));
    super::read_2bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

fn make_4bit_pixmap(bits_rect: &BitsRect) -> BitmapFour {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    output.set_palette(get_pict_palette(bits_rect));
    super::read_4bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

fn make_8bit_pixmap(bits_rect: &BitsRect) -> BitmapEight {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapEight::new(pixmap.width(), pixmap.height());
    output.set_palette(get_pict_palette(bits_rect));
    super::read_8bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

fn make_16bit_pixmap(bits_rect: &BitsRect) -> BitmapSixteen {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapSixteen::new(pixmap.width(), pixmap.height());
    // 16-bit pixmaps do not have color palettes
    super::read_16bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

fn make_32bit_pixmap(bits_rect: &BitsRect) -> BitmapTwentyFour {
    let pixmap = &bits_rect.pixMap;
    let mut output = BitmapTwentyFour::new(pixmap.width(), pixmap.height());
    // 32-bit pixmaps do not have color palettes
    super::read_32bit_bitmap_data(&mut output, &bits_rect.data.data, pixmap.row_bytes());
    output
}

enum Picture {
    V1(PictureV1),
    V2(PictureV2),
}

struct PictureV1 {
    picSize: u16,
    picFrame: Rect,
    picOps: Vec<PicV1Op>,
}

struct PictureV2 {
    picSize: u16,
    picFrame: Rect,
    picOps: Vec<PicV2Op>,
}

fn is_region_unsupported(rgn: &Region, bounds: &Rect) -> bool {
    (rgn.rgnSize != 10)
        || (rgn.rgnBBox.left > bounds.left)
        || (rgn.rgnBBox.top > bounds.top)
        || (rgn.rgnBBox.right < bounds.right)
        || (rgn.rgnBBox.bottom < bounds.bottom)
}

pub const QD_MODE_SRC_COPY: i16 = 0;
pub const QD_MODE_DITHER_COPY: i16 = 64;

fn is_transfer_mode_unsupported(mode: i16) -> bool {
    (mode != QD_MODE_SRC_COPY) && (mode != (QD_MODE_SRC_COPY + QD_MODE_DITHER_COPY))
}

fn issue_warnings_on_v1_opcode(
    pict_id: i16,
    picFrame: &Rect,
    opcode: &PicV1Op,
    tracing_mode: bool,
) {
    match opcode {
        &PicV1Op::Ignored(ignored) => {
            if tracing_mode {
                eprintln!(
                    "tracing: ignored opcode ${:02X} in PICT #{}",
                    ignored, pict_id
                );
            }
        }
        &PicV1Op::Skipped(skipped) => {
            if tracing_mode {
                eprintln!(
                    "tracing: skipped opcode ${:02X} in PICT #{}",
                    skipped, pict_id
                );
            }
        }
        PicV1Op::ClipRgn(clip_rgn) => {
            if is_region_unsupported(clip_rgn, picFrame) {
                eprintln!("warning: unsupported ClipRgn opcode in PICT #{}", pict_id);
            }
        }
        PicV1Op::BitsRect(data) => {
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported BitsRect transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV1Op::BitsRgn(data) => {
            if is_region_unsupported(&data.maskRgn, &data.srcRect) {
                eprintln!(
                    "warning: unsupported BitsRgn mask region in PICT #{}",
                    pict_id
                );
            }
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported BitsRgn transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV1Op::PackBitsRect(data) => {
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported PackBitsRect transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV1Op::PackBitsRgn(data) => {
            if is_region_unsupported(&data.maskRgn, &data.srcRect) {
                eprintln!(
                    "warning: unsupported PackBitsRgn mask region in PICT #{}",
                    pict_id
                );
            }
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported PackBitsRgn transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        _ => {}
    }
}

fn issue_warnings_on_v2_opcode(
    pict_id: i16,
    picFrame: &Rect,
    opcode: &PicV2Op,
    tracing_mode: bool,
) {
    match opcode {
        &PicV2Op::Ignored(ignored) => {
            if tracing_mode {
                eprintln!(
                    "tracing: ignored opcode ${:04X} in PICT #{}",
                    ignored, pict_id
                );
            }
        }
        &PicV2Op::Skipped(skipped) => {
            if skipped == 0x8200 || skipped == 0x8201 {
                eprintln!("warning: unsupported QuickTime data in PICT #{}", pict_id);
            } else if tracing_mode {
                eprintln!(
                    "tracing: skipped opcode ${:04X} in PICT #{}",
                    skipped, pict_id
                );
            }
        }
        PicV2Op::ClipRgn(clip_rgn) => {
            if is_region_unsupported(clip_rgn, picFrame) {
                eprintln!("warning: unsupported ClipRgn opcode in PICT #{}", pict_id);
            }
        }
        PicV2Op::BitsRect(data) => {
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported BitsRect transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV2Op::BitsRgn(data) => {
            if is_region_unsupported(&data.maskRgn, &data.srcRect) {
                eprintln!(
                    "warning: unsupported BitsRgn mask region in PICT #{}",
                    pict_id
                );
            }
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported BitsRgn transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV2Op::PackBitsRect(data) => {
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported PackBitsRect transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV2Op::PackBitsRgn(data) => {
            if is_region_unsupported(&data.maskRgn, &data.srcRect) {
                eprintln!(
                    "warning: unsupported PackBitsRgn mask region in PICT #{}",
                    pict_id
                );
            }
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported PackBitsRgn transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV2Op::DirectBitsRect(data) => {
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported DirectBitsRect transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        PicV2Op::DirectBitsRgn(data) => {
            if is_region_unsupported(&data.maskRgn, &data.srcRect) {
                eprintln!(
                    "warning: unsupported DirectBitsRgn mask region in PICT #{}",
                    pict_id
                );
            }
            if is_transfer_mode_unsupported(data.mode) {
                eprintln!(
                    "warning: unsupported DirectBitsRgn transfer mode ({}) in PICT #{}",
                    data.mode, pict_id
                );
            }
        }
        _ => {}
    }
}

impl Picture {
    fn read_from(
        mut reader: impl Read + Seek,
        pict_id: i16,
        tracing_mode: bool,
    ) -> io::Result<Option<Self>> {
        let picSize = reader.read_be_u16()?;
        let picFrame = Rect::read_from(&mut reader)?;
        let magic = reader.read_be_u16()?;
        if magic == 0x1101 {
            let mut picOps = Vec::new();
            while let Some(new_opcode) = PicV1Op::read_from(&mut reader)? {
                issue_warnings_on_v1_opcode(pict_id, &picFrame, &new_opcode, tracing_mode);
                picOps.push(new_opcode);
            }
            if !tracing_mode {
                let mut skipped_set = HashSet::new();
                for opcode in picOps.iter() {
                    if let &PicV1Op::Skipped(skipped) = opcode {
                        skipped_set.insert(skipped);
                    }
                }
                let mut skipped_vec = skipped_set.into_iter().collect::<Vec<_>>();
                skipped_vec.sort();
                if skipped_vec.len() != 0 {
                    eprintln!(
                        "warning: skipped unimplemented opcodes in PICT #{}",
                        pict_id
                    );
                    eprint!("note: ${:02X}", skipped_vec[0]);
                    for skipped in skipped_vec[1..].iter().copied() {
                        eprint!(", ${:02X}", skipped);
                    }
                    eprintln!();
                }
            }
            Ok(Some(Self::V1(PictureV1 {
                picSize,
                picFrame,
                picOps,
            })))
        } else if magic == 0x0011 {
            let version = reader.read_be_u16()?;
            if version != 0x02FF {
                return Ok(None);
            }
            let mut picOps = Vec::new();
            while let Some(new_opcode) = PicV2Op::read_from(&mut reader)? {
                issue_warnings_on_v2_opcode(pict_id, &picFrame, &new_opcode, tracing_mode);
                picOps.push(new_opcode);
            }
            if !tracing_mode {
                let mut skipped_set = HashSet::new();
                for opcode in picOps.iter() {
                    if let &PicV2Op::Skipped(skipped) = opcode {
                        if skipped != 0x8200 && skipped != 0x8201 {
                            skipped_set.insert(skipped);
                        }
                    }
                }
                let mut skipped_vec = skipped_set.into_iter().collect::<Vec<_>>();
                skipped_vec.sort();
                if skipped_vec.len() != 0 {
                    eprintln!(
                        "warning: skipped unimplemented opcodes in PICT #{}",
                        pict_id
                    );
                    eprint!("note: ${:04X}", skipped_vec[0]);
                    for skipped in skipped_vec[1..].iter().copied() {
                        eprint!(", ${:04X}", skipped);
                    }
                    eprintln!();
                }
            }
            Ok(Some(Self::V2(PictureV2 {
                picSize,
                picFrame,
                picOps,
            })))
        } else {
            Ok(None)
        }
    }
}

fn read_bytes(mut reader: impl Read, num: usize) -> io::Result<Vec<u8>> {
    let mut buffer = vec![0x00; num];
    reader.read_exact(&mut buffer).map(|_| buffer)
}

fn skip_bytes(mut reader: impl Seek, num: i64) -> io::Result<()> {
    reader.seek(SeekFrom::Current(num)).map(|_| ())
}

fn skip_v1_opcode(reader: impl Seek, opcode: u8, num: i64) -> io::Result<PicV1Op> {
    skip_bytes(reader, num).map(|_| PicV1Op::Skipped(opcode))
}

fn skip_v2_opcode(reader: impl Seek, opcode: u16, num: i64) -> io::Result<PicV2Op> {
    skip_bytes(reader, num).map(|_| PicV2Op::Skipped(opcode))
}

fn ignore_v1_opcode(reader: impl Seek, opcode: u8, num: i64) -> io::Result<PicV1Op> {
    skip_bytes(reader, num).map(|_| PicV1Op::Ignored(opcode))
}

fn ignore_v2_opcode(reader: impl Seek, opcode: u16, num: i64) -> io::Result<PicV2Op> {
    skip_bytes(reader, num).map(|_| PicV2Op::Ignored(opcode))
}

#[derive(Clone)]
struct OffsetOrigin {
    dh: i16,
    dv: i16,
}

impl OffsetOrigin {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let dh = reader.read_be_i16()?;
        let dv = reader.read_be_i16()?;
        Ok(Self { dh, dv })
    }
}

enum PicV1Op {
    // This is used for both the QuickDraw NOP opcode as well as the
    // comment opcodes.
    NOP,

    ClipRgn(Region),
    OffsetOrigin(OffsetOrigin),
    BitsRect(BitsRect),
    BitsRgn(BitsRgn),
    PackBitsRect(BitsRect),
    PackBitsRgn(BitsRgn),

    // This is used for opcodes which are unimplemented but don't affect
    // the picture's contents on their own.
    Ignored(u8),

    // This is used for opcodes which are unimplemented but do affect the
    // picture's contents. NOTE: If one of these is implemented, make sure
    // any "Ignored" opcode that affect the output are also implemented.
    Skipped(u8),
}

impl PicV1Op {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        let opcode = reader.read_be_u8()?;
        Ok(match opcode {
            0x00 => Some(PicV1Op::NOP),
            0x01 => Some(PicV1Op::ClipRgn(Region::read_from(reader)?)),
            0x02 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x03 => Some(skip_v1_opcode(reader, opcode, 2)?),
            0x04 => Some(skip_v1_opcode(reader, opcode, 1)?),
            0x05 => Some(skip_v1_opcode(reader, opcode, 2)?),
            0x06 => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x07 => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x08 => Some(skip_v1_opcode(reader, opcode, 2)?),
            0x09 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x0A => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x0B => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x0C => Some(PicV1Op::OffsetOrigin(OffsetOrigin::read_from(reader)?)),
            0x0D => Some(skip_v1_opcode(reader, opcode, 2)?),
            0x0E => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x0F => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x10 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x11 => Some(skip_v1_opcode(reader, opcode, 1)?),
            0x20 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x21 => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x22 => Some(skip_v1_opcode(reader, opcode, 6)?),
            0x23 => Some(skip_v1_opcode(reader, opcode, 2)?),
            0x28 => {
                skip_bytes(&mut reader, 4)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV1Op::Skipped(opcode))
            }
            0x29 | 0x2A => {
                skip_bytes(&mut reader, 1)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV1Op::Skipped(opcode))
            }
            0x2B => {
                skip_bytes(&mut reader, 2)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV1Op::Skipped(opcode))
            }
            0x30..=0x34 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x38..=0x3C => Some(PicV1Op::Skipped(opcode)),
            0x40..=0x44 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x48..=0x4C => Some(PicV1Op::Skipped(opcode)),
            0x50..=0x54 => Some(skip_v1_opcode(reader, opcode, 8)?),
            0x58..=0x5C => Some(PicV1Op::Skipped(opcode)),
            0x60..=0x64 => Some(skip_v1_opcode(reader, opcode, 12)?),
            0x68..=0x6C => Some(skip_v1_opcode(reader, opcode, 4)?),
            0x70..=0x74 => {
                let _ = Polygon::read_from(reader)?;
                Some(PicV1Op::Skipped(opcode))
            }
            0x78..=0x7C => Some(PicV1Op::Skipped(opcode)),
            0x80..=0x84 => {
                let _ = Region::read_from(reader)?;
                Some(PicV1Op::Skipped(opcode))
            }
            0x88..=0x8C => Some(PicV1Op::Skipped(opcode)),
            0x90 => Some(PicV1Op::BitsRect(BitsRect::read_from(reader)?)),
            0x91 => Some(PicV1Op::BitsRgn(BitsRgn::read_from(reader)?)),
            0x98 => Some(PicV1Op::PackBitsRect(BitsRect::read_from(reader)?)),
            0x99 => Some(PicV1Op::PackBitsRgn(BitsRgn::read_from(reader)?)),
            0xA0 => {
                skip_bytes(reader, 2)?;
                Some(PicV1Op::NOP)
            }
            0xA1 => {
                let _ = reader.read_be_u16()?;
                let length = reader.read_be_u16()?;
                skip_bytes(reader, length.into())?;
                Some(PicV1Op::NOP)
            }
            0xFF => None,
            _ => {
                return Err(io::Error::new(
                    ErrorKind::InvalidData,
                    format!("unimplemented PICT V1 opcode ${:02X}", opcode),
                ))
            }
        })
    }
}

enum PicV2Op {
    // This is used for both the QuickDraw NOP opcode as well as the
    // comment opcodes.
    NOP,

    ClipRgn(Region),
    OffsetOrigin(OffsetOrigin),
    BitsRect(BitsRect),
    BitsRgn(BitsRgn),
    PackBitsRect(BitsRect),
    PackBitsRgn(BitsRgn),
    DirectBitsRect(DirectBitsRect),
    DirectBitsRgn(DirectBitsRgn),

    // This is used for opcodes which are unimplemented but don't affect
    // the picture's contents on their own.
    Ignored(u16),

    // This is used for opcodes which are unimplemented but do affect the
    // picture's contents. NOTE: If one of these is implemented, make sure
    // any "Ignored" opcode that affect the output are also implemented.
    Skipped(u16),
}

impl PicV2Op {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        reader.align_to(2)?;
        let opcode = reader.read_be_u16()?;
        Ok(match opcode {
            0x0000 => Some(PicV2Op::NOP),
            0x0001 => Some(PicV2Op::ClipRgn(Region::read_from(reader)?)),
            0x0002 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0003 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x0004 => Some(skip_v2_opcode(reader, opcode, 1)?),
            0x0005 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x0006 => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x0007 => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x0008 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x0009 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x000A => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x000B => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x000C => Some(PicV2Op::OffsetOrigin(OffsetOrigin::read_from(reader)?)),
            0x000D => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x000E => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x000F => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x0010 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0011 => Some(skip_v2_opcode(reader, opcode, 1)?),
            // ($0012 ... $0014) pixel pattern opcodes not implemented here
            0x0015 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x0016 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x001A => Some(skip_v2_opcode(reader, opcode, 6)?),
            0x001B => Some(skip_v2_opcode(reader, opcode, 6)?),
            0x001C => Some(PicV2Op::Skipped(opcode)),
            0x001D => Some(ignore_v2_opcode(reader, opcode, 6)?),
            0x001E => Some(PicV2Op::Ignored(opcode)),
            0x001F => Some(ignore_v2_opcode(reader, opcode, 6)?),
            0x0020 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0021 => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x0022 => Some(skip_v2_opcode(reader, opcode, 6)?),
            0x0023 => Some(skip_v2_opcode(reader, opcode, 2)?),
            0x0024..=0x0027 => {
                let length = reader.read_be_u16()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x0028 => {
                skip_bytes(&mut reader, 4)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV2Op::Skipped(opcode))
            }
            0x0029 | 0x002A => {
                skip_bytes(&mut reader, 1)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV2Op::Skipped(opcode))
            }
            0x002B => {
                skip_bytes(&mut reader, 2)?;
                let _ = super::read_pstring(&mut reader)?;
                Some(PicV2Op::Skipped(opcode))
            }
            0x002C..=0x002F => {
                let length = reader.read_be_u16()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x0030..=0x0037 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0038..=0x003F => Some(PicV2Op::Skipped(opcode)),
            0x0040..=0x0047 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0048..=0x004F => Some(PicV2Op::Skipped(opcode)),
            0x0050..=0x0057 => Some(skip_v2_opcode(reader, opcode, 8)?),
            0x0058..=0x005F => Some(PicV2Op::Skipped(opcode)),
            0x0060..=0x0067 => Some(skip_v2_opcode(reader, opcode, 12)?),
            0x0068..=0x006F => Some(skip_v2_opcode(reader, opcode, 4)?),
            0x0070..=0x0077 => {
                let _ = Polygon::read_from(reader)?;
                Some(PicV2Op::Skipped(opcode))
            }
            0x0078..=0x007F => Some(PicV2Op::Skipped(opcode)),
            0x0080..=0x0087 => {
                let _ = Region::read_from(reader)?;
                Some(PicV2Op::Skipped(opcode))
            }
            0x0088..=0x008F => Some(PicV2Op::Skipped(opcode)),
            0x0090 => Some(PicV2Op::BitsRect(BitsRect::read_from(reader)?)),
            0x0091 => Some(PicV2Op::BitsRgn(BitsRgn::read_from(reader)?)),
            0x0092..=0x0097 => {
                let length = reader.read_be_u16()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x0098 => Some(PicV2Op::PackBitsRect(BitsRect::read_from(reader)?)),
            0x0099 => Some(PicV2Op::PackBitsRgn(BitsRgn::read_from(reader)?)),
            0x009A => Some(PicV2Op::DirectBitsRect(DirectBitsRect::read_from(reader)?)),
            0x009B => Some(PicV2Op::DirectBitsRgn(DirectBitsRgn::read_from(reader)?)),
            0x009C..=0x009F => {
                let length = reader.read_be_u16()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x00A0 => {
                skip_bytes(reader, 2)?;
                Some(PicV2Op::NOP)
            }
            0x00A1 => {
                let _ = reader.read_be_u16()?;
                let length = reader.read_be_u16()?;
                skip_bytes(reader, length.into())?;
                Some(PicV2Op::NOP)
            }
            0x00A2..=0x00AF => {
                let length = reader.read_be_u16()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x00B0..=0x00CF => Some(PicV2Op::Skipped(opcode)),
            0x00D0..=0x00FE => {
                let length = reader.read_be_u32()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x00FF => None,
            0x0100..=0x7FFF => {
                let length = (opcode >> 8) * 2;
                skip_bytes(reader, length.into())?;
                match opcode {
                    0x0C00 => Some(PicV2Op::NOP),
                    _ => Some(PicV2Op::Skipped(opcode)),
                }
            }
            0x8000..=0x80FF => Some(PicV2Op::Skipped(opcode)),
            0x8100..=0x81FF => {
                let length = reader.read_be_u32()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x8200 => {
                // Compressed QuickTime data
                let length = reader.read_be_u32()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0x8201 => {
                // Uncompressed QuickTime data
                let length = reader.read_be_u32()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            0xFFFF => {
                let length = reader.read_be_u32()?;
                Some(skip_v2_opcode(reader, opcode, length.into())?)
            }
            _ => {
                return Err(io::Error::new(
                    ErrorKind::InvalidData,
                    format!("unimplemented PICT V2 opcode ${:04X}", opcode),
                ))
            }
        })
    }
}

fn extract_bits_data_v1(pic_data: &[PicV1Op]) -> Vec<BitsRect> {
    let mut origin = Point { h: 0, v: 0 };
    let mut list = Vec::new();
    for opcode in pic_data {
        let data = match opcode {
            PicV1Op::BitsRect(data) => Some(data.clone()),
            PicV1Op::BitsRgn(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV1Op::PackBitsRect(data) => Some(data.clone()),
            PicV1Op::PackBitsRgn(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV1Op::OffsetOrigin(offset) => {
                origin.h += offset.dh;
                origin.v += offset.dv;
                None
            }
            _ => None,
        };
        if let Some(mut bits_rect) = data {
            bits_rect.dstRect.left -= origin.h;
            bits_rect.dstRect.top -= origin.v;
            bits_rect.dstRect.right -= origin.h;
            bits_rect.dstRect.bottom -= origin.v;
            list.push(bits_rect);
        }
    }
    list
}

fn extract_bits_data_v2(pic_data: &[PicV2Op]) -> Vec<BitsRect> {
    let mut origin = Point { h: 0, v: 0 };
    let mut list = Vec::new();
    for opcode in pic_data {
        let data = match opcode {
            PicV2Op::BitsRect(data) => Some(data.clone()),
            PicV2Op::BitsRgn(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::PackBitsRect(data) => Some(data.clone()),
            PicV2Op::PackBitsRgn(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::DirectBitsRect(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: ColorTable::default(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::DirectBitsRgn(data) => Some(BitsRect {
                pixMap: data.pixMap,
                ctTable: ColorTable::default(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::OffsetOrigin(offset) => {
                origin.h += offset.dh;
                origin.v += offset.dv;
                None
            }
            _ => None,
        };
        if let Some(mut bits_rect) = data {
            bits_rect.dstRect.left -= origin.h;
            bits_rect.dstRect.top -= origin.v;
            bits_rect.dstRect.right -= origin.h;
            bits_rect.dstRect.bottom -= origin.v;
            list.push(bits_rect);
        }
    }
    list
}

fn get_bitblt_params(picFrame: Rect, bits_rect: &BitsRect) -> (Point, Rect) {
    let dst_origin = Point {
        h: bits_rect.dstRect.left - picFrame.left,
        v: bits_rect.dstRect.top - picFrame.top,
    };
    let mut src_rect = bits_rect.srcRect;
    src_rect.left -= bits_rect.pixMap.bounds.left;
    src_rect.top -= bits_rect.pixMap.bounds.top;
    src_rect.right -= bits_rect.pixMap.bounds.left;
    src_rect.bottom -= bits_rect.pixMap.bounds.top;
    (dst_origin, src_rect)
}

fn convert_1bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapOne {
    let mut image = BitmapOne::new(picFrame.width() as _, picFrame.height() as _);
    image.set_palette(get_pict_palette(&bits_data[0]));
    for bits_rect in bits_data {
        let part = make_1bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_2bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapFour {
    let mut image = BitmapFour::new(picFrame.width() as _, picFrame.height() as _);
    image.set_palette(get_pict_palette(&bits_data[0]));
    for bits_rect in bits_data {
        let part = make_2bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_4bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapFour {
    let mut image = BitmapFour::new(picFrame.width() as _, picFrame.height() as _);
    image.set_palette(get_pict_palette(&bits_data[0]));
    for bits_rect in bits_data {
        let part = make_4bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_8bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapEight {
    let mut image = BitmapEight::new(picFrame.width() as _, picFrame.height() as _);
    image.set_palette(get_pict_palette(&bits_data[0]));
    for bits_rect in bits_data {
        let part = make_8bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_16bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapSixteen {
    let mut image = BitmapSixteen::new(picFrame.width() as _, picFrame.height() as _);
    // 16-bit PICTs do not have a color palette
    for bits_rect in bits_data {
        let part = make_16bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_32bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapTwentyFour {
    let mut image = BitmapTwentyFour::new(picFrame.width() as _, picFrame.height() as _);
    // 32-bit PICTs do not have a color palette
    for bits_rect in bits_data {
        let part = make_32bit_pixmap(bits_rect);
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_mixed_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapTwentyFour {
    let mut image = BitmapTwentyFour::new(picFrame.width() as _, picFrame.height() as _);
    // The mixed PICT will be a 32-bit PICT (or really, a 24-bit BMP),
    // so no color palette is needed for `image`.
    for bits_rect in bits_data {
        let part = match bits_rect.pixMap.pixelSize {
            1 => BitmapTwentyFour::from(make_1bit_pixmap(&bits_rect)),
            2 => BitmapTwentyFour::from(make_2bit_pixmap(&bits_rect)),
            4 => BitmapTwentyFour::from(make_4bit_pixmap(&bits_rect)),
            8 => BitmapTwentyFour::from(make_8bit_pixmap(&bits_rect)),
            16 => BitmapTwentyFour::from(make_16bit_pixmap(&bits_rect)),
            32 => BitmapTwentyFour::from(make_32bit_pixmap(&bits_rect)),
            _ => continue,
        };
        let (dst_origin, src_rect) = get_bitblt_params(picFrame, bits_rect);
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_pict(picFrame: Rect, bits_data: Vec<BitsRect>, writer: impl Write) -> io::Result<()> {
    if bits_data.is_empty() {
        return Err(io::Error::new(
            ErrorKind::InvalidData,
            "PICT does not contain any pixel data",
        ));
    }
    let depth = bits_data[0].pixMap.pixelSize;
    if bits_data
        .iter()
        .any(|data| data.is_incompatible_with(&bits_data[0]))
    {
        convert_mixed_pict(picFrame, &bits_data).write_bmp_file(writer)?;
        return Ok(());
    }
    match depth {
        1 => convert_1bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        2 => convert_2bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        4 => convert_4bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        8 => convert_8bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        16 => convert_16bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        32 => convert_32bit_pict(picFrame, &bits_data).write_bmp_file(writer)?,
        _ => {
            return Err(io::Error::new(
                ErrorKind::InvalidData,
                format!("PICT uses unsupported color depth {}", depth),
            ));
        }
    }
    Ok(())
}

fn convert_v1(picture: PictureV1, writer: impl Write) -> io::Result<()> {
    let bits_data = extract_bits_data_v1(&picture.picOps);
    convert_pict(picture.picFrame, bits_data, writer)
}

fn convert_v2(picture: PictureV2, writer: impl Write) -> io::Result<()> {
    let bits_data = extract_bits_data_v2(&picture.picOps);
    convert_pict(picture.picFrame, bits_data, writer)
}

pub fn convert(resource: &Resource, writer: impl Write, tracing_mode: bool) -> io::Result<()> {
    match Picture::read_from(io::Cursor::new(&resource.data), resource.id, tracing_mode)? {
        Some(Picture::V1(pict)) => convert_v1(pict, writer),
        Some(Picture::V2(pict)) => convert_v2(pict, writer),
        None => Ok(()),
    }
}
