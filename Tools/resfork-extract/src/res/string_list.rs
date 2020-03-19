use crate::mac_roman;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use crate::AnyResult;
use std::io::{self, Read, Seek, Write};
use zip::ZipWriter;

fn read_pstring(mut reader: impl Read) -> io::Result<String> {
    let length = reader.read_be_u8()?;
    let mut buffer = vec![0x00; length.into()];
    reader.read_exact(buffer.as_mut_slice())?;
    Ok(buffer.into_iter().map(mac_roman::decode).collect())
}

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

pub fn write_entry(res: &Resource, zipfile: &mut ZipWriter<impl Seek + Write>) -> AnyResult<()> {
    let string_list = StringList::read_from(res.data.as_slice())?;
    let mut output = String::new();
    for (idx, string) in string_list.strings.into_iter().enumerate() {
        output.push_str(&format!("{}: {:?}\n", idx + 1, string));
    }
    zipfile.start_file(format!("StringList/{}.txt", res.id), Default::default())?;
    zipfile.write_all(output.as_bytes())?;
    Ok(())
}
