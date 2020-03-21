use super::read_pstring;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct StringList {
    strings: Vec<String>,
}

impl StringList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let string_count = reader.read_be_u16()?;
        let mut strings = Vec::with_capacity(string_count.into());
        for _ in 0..string_count {
            strings.push(read_pstring(&mut reader)?);
        }
        Ok(Self { strings })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("StringList/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let string_list = StringList::read_from(data)?;
    for (idx, string) in string_list.strings.into_iter().enumerate() {
        writeln!(&mut writer, "{}: {:?}\n", idx + 1, string)?;
    }
    Ok(())
}
