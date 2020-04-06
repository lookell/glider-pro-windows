use super::{ColorTable, Point, Rect};
use crate::bitmap::{
    Bitmap, BitmapEight, BitmapFour, BitmapOne, BitmapSixteen, BitmapTwentyFour, RgbQuad,
};
use crate::rsrcfork::Resource;
use crate::utils::{ReadExt, SeekExt};
use std::io::{self, Read, Seek, Write};
use std::iter;

struct Region {
    rgnSize: u16,
    rgnBBox: Rect,
    rgnData: Vec<u8>,
}

impl Region {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let rgnSize = reader.read_be_u16()?;
        let rgnBBox = Rect::read_from(&mut reader)?;
        let mut rgnData = vec![0x00; (rgnSize - 10).into()];
        reader.read_exact(&mut rgnData)?;
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
        let mut polyData = vec![0x00; (polySize - 10).into()];
        reader.read_exact(&mut polyData)?;
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
    mode: u16,
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
            mode: reader.read_be_u16()?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

struct BitsRgn {
    pixMap: PixMap,
    ctTable: ColorTable,
    srcRect: Rect,
    dstRect: Rect,
    mode: u16,
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
            mode: reader.read_be_u16()?,
            maskRgn: Region::read_from(&mut reader)?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

struct DirectBitsRect {
    pixMap: PixMap,
    srcRect: Rect,
    dstRect: Rect,
    mode: u16,
    data: PixData,
}

impl DirectBitsRect {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let pixMap = PixMap::read_full(&mut reader)?;
        Ok(Self {
            pixMap,
            srcRect: Rect::read_from(&mut reader)?,
            dstRect: Rect::read_from(&mut reader)?,
            mode: reader.read_be_u16()?,
            data: PixData::read_from(&mut reader, &pixMap)?,
        })
    }
}

struct DirectBitsRgn {
    pixMap: PixMap,
    srcRect: Rect,
    dstRect: Rect,
    mode: u16,
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
            mode: reader.read_be_u16()?,
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
                let scanline = unpack_bits(&packed_data).expect("UnpackBits failed");
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
            _ => panic!(
                "unknown (packType, pixelSize) combo: ({}, {})",
                pixMap.packType, pixMap.pixelSize
            ),
        };
        if packType == 1 || pixMap.row_bytes() < 8 {
            let size = usize::from(pixMap.row_bytes()) * num_scanlines;
            let data = read_bytes(&mut reader, size)?;
            Ok(Self { data })
        } else if packType == 2 {
            let size = usize::from(pixMap.row_bytes()) * num_scanlines * 3 / 4;
            let packed_data = read_bytes(&mut reader, size)?;
            let mut data = Vec::with_capacity(size * 4 / 3);
            for chunk in packed_data.chunks(3) {
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
                let scanline = unpack_words(&packed_data).expect("UnpackBits failed");
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
                let scanline = unpack_bits(&packed_data).expect("UnpackBits failed");
                assert!(scanline.len() % 3 == 0);
                let rest = &scanline;
                let (red_scanline, rest) = rest.split_at(scanline.len() / 3);
                let (green_scanline, rest) = rest.split_at(scanline.len() / 3);
                let (blue_scanline, _) = rest.split_at(scanline.len() / 3);
                for idx in 0..(scanline.len() / 3) {
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
            unimplemented!("PixData::direct_read_from")
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

fn make_1bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapOne {
    let mut output = BitmapOne::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (8 * xbase) as u16;
            output.set_pixel(xbase, y, ((byte & 0x80) == 0).into());
            output.set_pixel(xbase + 1, y, ((byte & 0x40) == 0).into());
            output.set_pixel(xbase + 2, y, ((byte & 0x20) == 0).into());
            output.set_pixel(xbase + 3, y, ((byte & 0x10) == 0).into());
            output.set_pixel(xbase + 4, y, ((byte & 0x08) == 0).into());
            output.set_pixel(xbase + 5, y, ((byte & 0x04) == 0).into());
            output.set_pixel(xbase + 6, y, ((byte & 0x02) == 0).into());
            output.set_pixel(xbase + 7, y, ((byte & 0x01) == 0).into());
        }
    }
    output
}

fn make_4bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapFour {
    let mut output = BitmapFour::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        let y = y as u16;
        for (xbase, byte) in row.iter().copied().enumerate() {
            let xbase = (2 * xbase) as u16;
            output.set_pixel(xbase, y, 15 - (byte / 16));
            output.set_pixel(xbase + 1, y, 15 - (byte % 16));
        }
    }
    output
}

fn make_8bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapEight {
    let mut output = BitmapEight::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        for (x, byte) in row.iter().copied().enumerate() {
            output.set_pixel(x as _, y as _, 255 - byte);
        }
    }
    output
}

fn make_16bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapSixteen {
    let mut output = BitmapSixteen::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        for (x, bytes) in row.chunks_exact(2).enumerate() {
            let color = RgbQuad::new(
                8 * ((bytes[0] & 0x7A) >> 2),
                8 * (((bytes[0] & 0x03) << 3) | ((bytes[1] & 0xE0) >> 5)),
                8 * (bytes[1] & 0x1F),
            );
            output.set_pixel(x as _, y as _, color);
        }
    }
    output
}

fn make_32bit_pixmap(pixmap: &PixMap, bits: &[u8]) -> BitmapTwentyFour {
    let mut output = BitmapTwentyFour::new(pixmap.width(), pixmap.height());
    let row_bytes = usize::from(pixmap.row_bytes());
    for (y, row) in bits.chunks_exact(row_bytes).enumerate() {
        for (x, bytes) in row.chunks_exact(4).enumerate() {
            output.set_pixel(x as _, y as _, RgbQuad::new(bytes[1], bytes[2], bytes[3]));
        }
    }
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

impl Picture {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        let picSize = reader.read_be_u16()?;
        let picFrame = Rect::read_from(&mut reader)?;
        let magic = reader.read_be_u16()?;
        if magic == 0x1101 {
            let closure = || PicV1Op::read_from(&mut reader).ok().flatten();
            Ok(Some(Self::V1(PictureV1 {
                picSize,
                picFrame,
                picOps: iter::from_fn(closure).collect(),
            })))
        } else if magic == 0x0011 {
            let version = reader.read_be_u16()?;
            if version != 0x02FF {
                return Ok(None);
            }
            let closure = || PicV2Op::read_from(&mut reader).ok().flatten();
            Ok(Some(Self::V2(PictureV2 {
                picSize,
                picFrame,
                picOps: iter::from_fn(closure).collect(),
            })))
        } else {
            Ok(None)
        }
    }
}

enum PicV1Op {
    NOP,
    Clip(Region),
    BitsRect(BitsRect),
    BitsRgn(BitsRgn),
    PackBitsRect(BitsRect),
    PackBitsRgn(BitsRgn),
    ShortComment(u16),
}

impl PicV1Op {
    fn read_from(mut reader: impl Read) -> io::Result<Option<Self>> {
        let opcode = reader.read_be_u8()?;
        Ok(match opcode {
            0x00 => Some(Self::NOP),
            0x01 => Some(Self::Clip(Region::read_from(reader)?)),
            0x90 => Some(Self::BitsRect(BitsRect::read_from(reader)?)),
            0x91 => Some(Self::BitsRgn(BitsRgn::read_from(reader)?)),
            0x98 => Some(Self::PackBitsRect(BitsRect::read_from(reader)?)),
            0x99 => Some(Self::PackBitsRgn(BitsRgn::read_from(reader)?)),
            0xA0 => Some(Self::ShortComment(reader.read_be_u16()?)),
            0xFF => None,
            _ => unimplemented!("PICT V1 opcode: ${:02X}", opcode),
        })
    }
}

enum PicV2Op {
    NOP,
    Clip(Region),
    DefHilite,
    BitsRect(BitsRect),
    BitsRgn(BitsRgn),
    PackBitsRect(BitsRect),
    PackBitsRgn(BitsRgn),
    DirectBitsRect(DirectBitsRect),
    DirectBitsRgn(DirectBitsRgn),
    ShortComment(u16),
    HeaderOp(Vec<u8>),
}

fn read_bytes(mut reader: impl Read, num: usize) -> io::Result<Vec<u8>> {
    let mut buffer = vec![0x00; num];
    reader.read_exact(&mut buffer).map(|_| buffer)
}

impl PicV2Op {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        reader.align_to(2)?;
        let opcode = reader.read_be_u16()?;
        Ok(match opcode {
            0x0000 => Some(Self::NOP),
            0x0001 => Some(Self::Clip(Region::read_from(reader)?)),
            0x001E => Some(Self::DefHilite),
            0x0090 => Some(Self::BitsRect(BitsRect::read_from(reader)?)),
            0x0091 => Some(Self::BitsRgn(BitsRgn::read_from(reader)?)),
            0x0098 => Some(Self::PackBitsRect(BitsRect::read_from(reader)?)),
            0x0099 => Some(Self::PackBitsRgn(BitsRgn::read_from(reader)?)),
            0x009A => Some(Self::DirectBitsRect(DirectBitsRect::read_from(reader)?)),
            0x009B => Some(Self::DirectBitsRgn(DirectBitsRgn::read_from(reader)?)),
            0x00A0 => Some(Self::ShortComment(reader.read_be_u16()?)),
            0x00FF => None,
            0x0C00 => Some(Self::HeaderOp(read_bytes(reader, 24)?)),
            _ => unimplemented!("PICT V2 opcode ${:04X}", opcode),
        })
    }
}

fn extract_bits_data_v1(pic_data: &[PicV1Op]) -> Vec<BitsRect> {
    let mut list = Vec::new();
    for opcode in pic_data {
        match opcode {
            PicV1Op::BitsRect(data) => list.push(data.clone()),
            PicV1Op::BitsRgn(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV1Op::PackBitsRect(data) => list.push(data.clone()),
            PicV1Op::PackBitsRgn(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            _ => {}
        }
    }
    list
}

fn extract_bits_data_v2(pic_data: &[PicV2Op]) -> Vec<BitsRect> {
    let mut list = Vec::new();
    for opcode in pic_data {
        match opcode {
            PicV2Op::BitsRect(data) => list.push(data.clone()),
            PicV2Op::BitsRgn(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::PackBitsRect(data) => list.push(data.clone()),
            PicV2Op::PackBitsRgn(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: data.ctTable.clone(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::DirectBitsRect(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: ColorTable::default(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            PicV2Op::DirectBitsRgn(data) => list.push(BitsRect {
                pixMap: data.pixMap,
                ctTable: ColorTable::default(),
                srcRect: data.srcRect,
                dstRect: data.dstRect,
                mode: data.mode,
                data: data.data.clone(),
            }),
            _ => {}
        }
    }
    list
}

fn convert_1bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapOne {
    let mut image = BitmapOne::new(picFrame.width() as _, picFrame.height() as _);
    for bits_rect in bits_data {
        let part = make_1bit_pixmap(&bits_rect.pixMap, &bits_rect.data.data);
        let mut src_rect = bits_rect.srcRect;
        src_rect.left -= bits_rect.pixMap.bounds.left;
        src_rect.top -= bits_rect.pixMap.bounds.top;
        src_rect.right -= bits_rect.pixMap.bounds.left;
        src_rect.bottom -= bits_rect.pixMap.bounds.top;
        let mut dst_origin = Point::new(bits_rect.dstRect.left, bits_rect.dstRect.top);
        dst_origin.h -= picFrame.left;
        dst_origin.v -= picFrame.top;
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_4bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapFour {
    let mut image = BitmapFour::new(picFrame.width() as _, picFrame.height() as _);
    for bits_rect in bits_data {
        let part = make_4bit_pixmap(&bits_rect.pixMap, &bits_rect.data.data);
        let mut src_rect = bits_rect.srcRect;
        src_rect.left -= bits_rect.pixMap.bounds.left;
        src_rect.top -= bits_rect.pixMap.bounds.top;
        src_rect.right -= bits_rect.pixMap.bounds.left;
        src_rect.bottom -= bits_rect.pixMap.bounds.top;
        let mut dst_origin = Point::new(bits_rect.dstRect.left, bits_rect.dstRect.top);
        dst_origin.h -= picFrame.left;
        dst_origin.v -= picFrame.top;
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_8bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapEight {
    let mut image = BitmapEight::new(picFrame.width() as _, picFrame.height() as _);
    for bits_rect in bits_data {
        let part = make_8bit_pixmap(&bits_rect.pixMap, &bits_rect.data.data);
        let mut src_rect = bits_rect.srcRect;
        src_rect.left -= bits_rect.pixMap.bounds.left;
        src_rect.top -= bits_rect.pixMap.bounds.top;
        src_rect.right -= bits_rect.pixMap.bounds.left;
        src_rect.bottom -= bits_rect.pixMap.bounds.top;
        let mut dst_origin = Point::new(bits_rect.dstRect.left, bits_rect.dstRect.top);
        dst_origin.h -= picFrame.left;
        dst_origin.v -= picFrame.top;
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_16bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapSixteen {
    let mut image = BitmapSixteen::new(picFrame.width() as _, picFrame.height() as _);
    for bits_rect in bits_data {
        let part = make_16bit_pixmap(&bits_rect.pixMap, &bits_rect.data.data);
        let mut src_rect = bits_rect.srcRect;
        src_rect.left -= bits_rect.pixMap.bounds.left;
        src_rect.top -= bits_rect.pixMap.bounds.top;
        src_rect.right -= bits_rect.pixMap.bounds.left;
        src_rect.bottom -= bits_rect.pixMap.bounds.top;
        let mut dst_origin = Point::new(bits_rect.dstRect.left, bits_rect.dstRect.top);
        dst_origin.h -= picFrame.left;
        dst_origin.v -= picFrame.top;
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_32bit_pict(picFrame: Rect, bits_data: &[BitsRect]) -> BitmapTwentyFour {
    let mut image = BitmapTwentyFour::new(picFrame.width() as _, picFrame.height() as _);
    for bits_rect in bits_data {
        let part = make_32bit_pixmap(&bits_rect.pixMap, &bits_rect.data.data);
        let mut src_rect = bits_rect.srcRect;
        src_rect.left -= bits_rect.pixMap.bounds.left;
        src_rect.top -= bits_rect.pixMap.bounds.top;
        src_rect.right -= bits_rect.pixMap.bounds.left;
        src_rect.bottom -= bits_rect.pixMap.bounds.top;
        let mut dst_origin = Point::new(bits_rect.dstRect.left, bits_rect.dstRect.top);
        dst_origin.h -= picFrame.left;
        dst_origin.v -= picFrame.top;
        image.bitblt(&part, dst_origin, src_rect);
    }
    image
}

fn convert_pict(
    picFrame: Rect,
    bits_data: Vec<BitsRect>,
    mut writer: impl Write,
) -> io::Result<()> {
    if bits_data.is_empty() {
        let mut image = BitmapOne::new(picFrame.width() as _, picFrame.height() as _);
        image.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
        image.write_bmp_file(&mut writer)?;
        return Ok(());
    }
    let depth = bits_data[0].pixMap.pixelSize;
    let mut palette = match depth {
        1 => vec![RgbQuad::BLACK; 2],
        4 => vec![RgbQuad::BLACK; 16],
        8 => vec![RgbQuad::BLACK; 256],
        _ => Vec::new(),
    };
    palette
        .iter_mut()
        .rev()
        .zip(&bits_data[0].ctTable.ctTable)
        .for_each(|(dst, src)| *dst = src.rgb.into());
    if bits_data[0].ctTable.ctTable.is_empty() && depth == 1 {
        palette = vec![RgbQuad::BLACK, RgbQuad::WHITE];
    }
    match depth {
        1 => {
            let mut image = convert_1bit_pict(picFrame, &bits_data);
            image.set_palette(palette.into_iter());
            image.write_bmp_file(&mut writer)?;
        }
        4 => {
            let mut image = convert_4bit_pict(picFrame, &bits_data);
            image.set_palette(palette.into_iter());
            image.write_bmp_file(&mut writer)?;
        }
        8 => {
            let mut image = convert_8bit_pict(picFrame, &bits_data);
            image.set_palette(palette.into_iter());
            image.write_bmp_file(&mut writer)?;
        }
        16 => {
            let image = convert_16bit_pict(picFrame, &bits_data);
            image.write_bmp_file(&mut writer)?;
        }
        32 => {
            let image = convert_32bit_pict(picFrame, &bits_data);
            image.write_bmp_file(&mut writer)?;
        }
        _ => {
            let mut image = BitmapOne::new(picFrame.width() as _, picFrame.height() as _);
            image.set_palette([RgbQuad::BLACK, RgbQuad::WHITE].iter().copied());
            image.write_bmp_file(&mut writer)?;
        }
    }
    Ok(())
}

fn convert_v1(picture: PictureV1, writer: impl Write) -> io::Result<()> {
    convert_pict(
        picture.picFrame,
        extract_bits_data_v1(&picture.picOps),
        writer,
    )
}

fn convert_v2(picture: PictureV2, writer: impl Write) -> io::Result<()> {
    convert_pict(
        picture.picFrame,
        extract_bits_data_v2(&picture.picOps),
        writer,
    )
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Picture/{}.bmp", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    match Picture::read_from(io::Cursor::new(data))? {
        Some(Picture::V1(pict)) => convert_v1(pict, writer),
        Some(Picture::V2(pict)) => convert_v2(pict, writer),
        None => Ok(()),
    }
}
