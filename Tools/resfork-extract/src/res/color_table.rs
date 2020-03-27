use super::ColorSpec;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct ColorTable {
    ctSeed: i32,
    ctFlags: i16,
    ctSize: i16,
    ctTable: Vec<ColorSpec>,
}

impl ColorTable {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
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
            ctSize,
            ctTable,
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("ColorTable/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let clut = ColorTable::read_from(data)?;
    for entry in clut.ctTable {
        let r = entry.rgb.red >> 8;
        let g = entry.rgb.green >> 8;
        let b = entry.rgb.blue >> 8;
        writeln!(
            &mut writer,
            "color {}: rgb({}, {}, {});",
            entry.value, r, g, b
        )?;
    }
    Ok(())
}
