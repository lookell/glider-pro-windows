#![allow(dead_code)]
#![allow(nonstandard_style)]

use crate::mac_roman;
use crate::utils::ReadExt;
use std::io::{self, Read};

pub mod alert; // 'ALRT'
pub mod animated_cursor; // 'acur'
pub mod bundle; // 'BNDL'
pub mod color_cursor; // 'crsr'
pub mod color_icon; // 'cicn'
pub mod color_table; // 'clut'
pub mod control; // 'CNTL'
pub mod control_color_table; // 'cctb'
pub mod cursor; // 'CURS'
pub mod dialog; // 'DLOG'
pub mod dialog_color_table; // 'dctb'
pub mod file_reference; // 'FREF'
pub mod icon; // 'ICON'
pub mod icon_list; // 'ICN#'
pub mod item_list; // 'DITL'
pub mod large_4bit_icon; // 'icl4'
pub mod large_8bit_icon; // 'icl8'
pub mod pattern_list; // 'PAT#'
pub mod small_4bit_icon; // 'ics4'
pub mod small_8bit_icon; // 'ics8'
pub mod small_icon_list; // 'ics#'
pub mod string_list; // 'STR#'
pub mod version; // 'vers'
pub mod window; // 'WIND'
pub mod window_color_table; // 'wctb'

#[derive(Clone, Copy)]
pub struct Point {
    pub v: i16,
    pub h: i16,
}

impl Point {
    pub fn new(h: i16, v: i16) -> Self {
        Self { v, h }
    }

    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            v: reader.read_be_i16()?,
            h: reader.read_be_i16()?,
        })
    }
}

#[derive(Clone, Copy)]
pub struct Rect {
    pub top: i16,
    pub left: i16,
    pub bottom: i16,
    pub right: i16,
}

impl Rect {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            top: reader.read_be_i16()?,
            left: reader.read_be_i16()?,
            bottom: reader.read_be_i16()?,
            right: reader.read_be_i16()?,
        })
    }

    pub fn width(self) -> i32 {
        i32::from(self.right) - i32::from(self.left)
    }

    pub fn height(self) -> i32 {
        i32::from(self.bottom) - i32::from(self.top)
    }
}

pub fn read_pstring(mut reader: impl Read) -> io::Result<String> {
    let length = reader.read_be_u8()?;
    let mut buffer = vec![0x00; length.into()];
    reader.read_exact(buffer.as_mut_slice())?;
    Ok(buffer.into_iter().map(mac_roman::decode).collect())
}

#[derive(Clone, Copy, Eq, PartialEq)]
pub struct RGBColor {
    pub red: u16,
    pub green: u16,
    pub blue: u16,
}

impl RGBColor {
    pub const fn new(red: u16, green: u16, blue: u16) -> Self {
        Self { red, green, blue }
    }

    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            red: reader.read_be_u16()?,
            green: reader.read_be_u16()?,
            blue: reader.read_be_u16()?,
        })
    }
}

#[derive(Clone, Copy)]
pub struct ColorSpec {
    pub value: i16,
    pub rgb: RGBColor,
}

impl ColorSpec {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            value: reader.read_be_i16()?,
            rgb: RGBColor::read_from(&mut reader)?,
        })
    }
}

#[derive(Clone)]
pub struct ColorTable {
    pub ctSeed: i32,
    pub ctFlags: i16,
    pub ctTable: Vec<ColorSpec>,
}

impl ColorTable {
    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let ctSeed = reader.read_be_i32()?;
        let ctFlags = reader.read_be_i16()?;
        let ctSize = reader.read_be_i16()?.wrapping_add(1);
        let mut ctTable = Vec::with_capacity((ctSize.max(0) as u16).into());
        for _ in 0..ctSize {
            ctTable.push(ColorSpec::read_from(&mut reader)?);
        }
        Ok(Self {
            ctSeed,
            ctFlags,
            ctTable,
        })
    }
}
