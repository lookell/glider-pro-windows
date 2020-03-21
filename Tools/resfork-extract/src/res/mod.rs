#![allow(nonstandard_style)]

use crate::utils::ReadExt;
use std::io::{self, Read};

pub mod dialog_color_table;
pub mod string_list;
pub mod window_color_table;

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
