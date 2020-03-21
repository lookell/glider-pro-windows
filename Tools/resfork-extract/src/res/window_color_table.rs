use super::ColorSpec;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use crate::AnyResult;
use std::io::{self, Read, Write};

struct WinCTab {
    wCSeed: i32,
    wCReserved: i16,
    ctSize: i16,
    ctTable: Vec<ColorSpec>,
}

impl WinCTab {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let wCSeed = reader.read_be_i32()?;
        let wCReserved = reader.read_be_i16()?;
        let ctSize = reader.read_be_i16()?;
        let mut ctTable = Vec::with_capacity((ctSize.max(0) as u16).into());
        for _ in 0..ctSize {
            ctTable.push(ColorSpec::read_from(&mut reader)?);
        }
        Ok(Self {
            wCSeed,
            wCReserved,
            ctSize,
            ctTable,
        })
    }
}

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

pub fn convert(data: &[u8], mut writer: impl Write) -> AnyResult<()> {
    let clut = WinCTab::read_from(data)?;
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
        let r = entry.rgb.red >> 8;
        let g = entry.rgb.green >> 8;
        let b = entry.rgb.blue >> 8;
        writeln!(&mut writer, ": rgb({}, {}, {});", r, g, b)?;
    }
    Ok(())
}
