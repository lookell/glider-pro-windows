use crate::res::window_color_table;
use std::io::{self, Write};

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    window_color_table::convert(data, writer)
}
