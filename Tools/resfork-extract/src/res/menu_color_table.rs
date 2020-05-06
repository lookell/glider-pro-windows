use super::RGBColor;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct MCEntry {
    mctID: i16,
    mctItem: i16,
    mctRGB1: RGBColor,
    mctRGB2: RGBColor,
    mctRGB3: RGBColor,
    mctRGB4: RGBColor,
    mctReserved: i16,
}

impl MCEntry {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            mctID: reader.read_be_i16()?,
            mctItem: reader.read_be_i16()?,
            mctRGB1: RGBColor::read_from(&mut reader)?,
            mctRGB2: RGBColor::read_from(&mut reader)?,
            mctRGB3: RGBColor::read_from(&mut reader)?,
            mctRGB4: RGBColor::read_from(&mut reader)?,
            mctReserved: reader.read_be_i16()?,
        })
    }
}

struct MenuColorTable {
    table: Vec<MCEntry>,
}

impl MenuColorTable {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let numEntries = reader.read_be_u16()?;
        let mut table = Vec::with_capacity(numEntries.into());
        for _ in 0..numEntries {
            table.push(MCEntry::read_from(&mut reader)?);
        }
        Ok(Self { table })
    }
}

fn rgb(rgb: RGBColor) -> String {
    format!(
        "rgb(${:04X}, ${:04X}, ${:04X})",
        rgb.red, rgb.green, rgb.blue
    )
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let table = MenuColorTable::read_from(data)?.table;
    for entry in table {
        match (entry.mctID, entry.mctItem) {
            (0, 0) => {
                writeln!(&mut writer, "menu bar colors:")?;
                writeln!(&mut writer, "\tdefault title text: {}", rgb(entry.mctRGB1))?;
                writeln!(
                    &mut writer,
                    "\tdefault menu background: {}",
                    rgb(entry.mctRGB2)
                )?;
                writeln!(&mut writer, "\tdefault items text: {}", rgb(entry.mctRGB3))?;
                writeln!(
                    &mut writer,
                    "\tdefault menubar background: {}",
                    rgb(entry.mctRGB4)
                )?;
            }
            (-99, _) => {}
            (id, 0) => {
                writeln!(&mut writer, "menu title colors ({}):", id)?;
                writeln!(&mut writer, "\ttitle text: {}", rgb(entry.mctRGB1))?;
                writeln!(&mut writer, "\tmenubar background: {}", rgb(entry.mctRGB2))?;
                writeln!(&mut writer, "\titem text: {}", rgb(entry.mctRGB3))?;
                writeln!(&mut writer, "\tmenu background: {}", rgb(entry.mctRGB4))?;
            }
            (id, item) => {
                writeln!(
                    &mut writer,
                    "menu item colors (menu {}, item {}):",
                    id, item
                )?;
                writeln!(&mut writer, "\tmark: {}", rgb(entry.mctRGB1))?;
                writeln!(&mut writer, "\tname: {}", rgb(entry.mctRGB2))?;
                writeln!(&mut writer, "\taccelerator: {}", rgb(entry.mctRGB3))?;
                writeln!(&mut writer, "\tbackground: {}", rgb(entry.mctRGB4))?;
            }
        }
    }
    Ok(())
}
