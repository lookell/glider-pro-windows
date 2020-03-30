use super::ColorTable;
use crate::rsrcfork::Resource;
use std::io::{self, Write};

const CONTENT_COLOR: i16 = 0;
const FRAME_COLOR: i16 = 1;
const TEXT_COLOR: i16 = 2;
const HILITE_COLOR: i16 = 3;
const TITLE_BAR_COLOR: i16 = 4;
const HILITE_LIGHT: i16 = 5;
const HILITE_DARK: i16 = 6;
const TITLE_BAR_LIGHT: i16 = 7;
const TITLE_BAR_DARK: i16 = 8;
const DIALOG_LIGHT: i16 = 9;
const DIALOG_DARK: i16 = 10;
const TINGE_LIGHT: i16 = 11;
const TINGE_DARK: i16 = 12;

pub fn get_entry_name(res: &Resource) -> String {
    format!("WindowColorTable/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let clut = ColorTable::read_from(data)?;
    for entry in clut.ctTable.into_iter() {
        match entry.value {
            CONTENT_COLOR => write!(&mut writer, "wContentColor")?,
            FRAME_COLOR => write!(&mut writer, "wFrameColor")?,
            TEXT_COLOR => write!(&mut writer, "wTextColor")?,
            HILITE_COLOR => write!(&mut writer, "wHiliteColor")?,
            TITLE_BAR_COLOR => write!(&mut writer, "wTitleBar")?,
            HILITE_LIGHT => write!(&mut writer, "wHiliteLight")?,
            HILITE_DARK => write!(&mut writer, "wHiliteDark")?,
            TITLE_BAR_LIGHT => write!(&mut writer, "wTitleBarLight")?,
            TITLE_BAR_DARK => write!(&mut writer, "wTitleBarDark")?,
            DIALOG_LIGHT => write!(&mut writer, "wDialogLight")?,
            DIALOG_DARK => write!(&mut writer, "wDialogDark")?,
            TINGE_LIGHT => write!(&mut writer, "wTingeLight")?,
            TINGE_DARK => write!(&mut writer, "wTingeDark")?,
            _ => write!(&mut writer, "[color #{}]", entry.value)?,
        }
        writeln!(
            &mut writer,
            ": rgb(${:04X}, ${:04X}, ${:04X});",
            entry.rgb.red, entry.rgb.green, entry.rgb.blue
        )?;
    }
    Ok(())
}
