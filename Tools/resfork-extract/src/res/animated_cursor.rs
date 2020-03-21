use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct Acur {
    n: i16,
    index: i16,
    frame: Vec<AcurEntry>,
}

struct AcurEntry {
    resID: i16,
    reserved: i16,
}

impl Acur {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let n = reader.read_be_i16()?;
        let index = reader.read_be_i16()?;
        let mut frame = Vec::with_capacity((n.max(0) as u16).into());
        for _ in 0..n {
            frame.push(AcurEntry::read_from(&mut reader)?);
        }
        Ok(Self { n, index, frame })
    }
}

impl AcurEntry {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            resID: reader.read_be_i16()?,
            reserved: reader.read_be_i16()?,
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("AnimatedCursor/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let acur = Acur::read_from(data)?;
    for (idx, entry) in acur.frame.into_iter().enumerate() {
        writeln!(
            &mut writer,
            "cursor entry {} uses resource ID ({})",
            idx + 1,
            entry.resID
        )?;
    }
    Ok(())
}
