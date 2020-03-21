use crate::res::window_color_table;
use crate::rsrcfork::Resource;
use crate::AnyResult;
use std::io::Write;

pub fn get_entry_name(res: &Resource) -> String {
    format!("DialogColorTable/{}.txt", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> AnyResult<()> {
    window_color_table::convert(data, writer)
}
