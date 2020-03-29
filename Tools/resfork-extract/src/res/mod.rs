#![allow(dead_code)]
#![allow(nonstandard_style)]

use crate::mac_roman;
use crate::utils::ReadExt;
use std::io::{self, Read};

pub mod alert; // 'ALRT'
pub mod animated_cursor; // 'acur'
pub mod color_table; // 'clut'
pub mod control; // 'CNTL'
pub mod control_color_table; // 'cctb'
pub mod cursor; // 'CURS'
pub mod dialog; // 'DLOG'
pub mod dialog_color_table; // 'dctb'
pub mod icon; // 'ICON'
pub mod item_list; // 'DITL'
pub mod string_list; // 'STR#'
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

#[derive(Clone, Copy)]
pub struct RGBColor {
    pub red: u16,
    pub green: u16,
    pub blue: u16,
}

impl RGBColor {
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
