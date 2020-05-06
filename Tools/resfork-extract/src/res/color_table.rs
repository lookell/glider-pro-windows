use super::ColorTable;
use std::io::{self, Write};

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let clut = ColorTable::read_from(data)?;
    for entry in clut.ctTable {
        writeln!(
            &mut writer,
            "color {}: rgb(${:04X}, ${:04X}, ${:04X});",
            entry.value, entry.rgb.red, entry.rgb.green, entry.rgb.blue
        )?;
    }
    Ok(())
}
