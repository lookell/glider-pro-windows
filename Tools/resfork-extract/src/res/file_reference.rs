use crate::rsrcfork::ResType;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct FileReference {
    file_type: ResType,
    icon_id: i16,
    _ignored: String,
}

impl FileReference {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            file_type: ResType::read_from(&mut reader)?,
            icon_id: reader.read_be_i16()?,
            _ignored: super::read_pstring(&mut reader)?,
        })
    }
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let file_ref = FileReference::read_from(data)?;
    writeln!(
        writer,
        "type '{}' uses icon ({})",
        file_ref.file_type, file_ref.icon_id
    )
}
