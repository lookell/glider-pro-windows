use crate::mac_roman;
use std::io::{self, Write};

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let string: String = data.iter().copied().map(mac_roman::decode).collect();
    writer.write_all(string.as_bytes())?;
    Ok(())
}
