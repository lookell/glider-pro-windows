use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct Version {
    majorRev: u8,
    minorAndBugRev: u8,
    stage: u8,
    nonRelRev: u8,
    countryCode: i16,
    shortVersion: String,
    longVersion: String,
}

impl Version {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            majorRev: reader.read_be_u8()?,
            minorAndBugRev: reader.read_be_u8()?,
            stage: reader.read_be_u8()?,
            nonRelRev: reader.read_be_u8()?,
            countryCode: reader.read_be_i16()?,
            shortVersion: super::read_pstring(&mut reader)?,
            longVersion: super::read_pstring(&mut reader)?,
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Version/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let version = Version::read_from(data)?;
    writeln!(&mut writer, "majorRev: 0x{:02X}", version.majorRev)?;
    writeln!(
        &mut writer,
        "minorAndBugRev: 0x{:02X}",
        version.minorAndBugRev
    )?;
    match version.stage {
        0x20 => writeln!(&mut writer, "stage: development")?,
        0x40 => writeln!(&mut writer, "stage: alpha")?,
        0x60 => writeln!(&mut writer, "stage: beta")?,
        0x80 => writeln!(&mut writer, "stage: release")?,
        _ => writeln!(&mut writer, "stage: 0x{:02X}", version.stage)?,
    }
    writeln!(&mut writer, "nonRelRev: 0x{:02X}", version.minorAndBugRev)?;
    writeln!(&mut writer, "countryCode: {}", version.countryCode)?;
    writeln!(&mut writer, "shortVersion: {:?}", version.shortVersion)?;
    writeln!(&mut writer, "longVersion: {:?}", version.longVersion)?;
    Ok(())
}
