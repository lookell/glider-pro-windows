use crate::res::window_color_table;
use crate::rsrcfork::Resource;
use std::io::{self, Write};

pub fn get_entry_name(res: &Resource) -> String {
    format!("DialogColorTable/{}.txt", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    window_color_table::convert(data, writer)
}
