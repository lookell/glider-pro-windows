use super::ColorSpec;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct CtlCTab {
    ccSeed: i32,
    ccRider: i16,
    ctSize: i16,
    ctTable: Vec<ColorSpec>,
}

impl CtlCTab {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let ccSeed = reader.read_be_i32()?;
        let ccRider = reader.read_be_i16()?;
        let ctSize = reader.read_be_i16()?.wrapping_add(1);
        let mut ctTable = Vec::with_capacity((ctSize.max(0) as u16).into());
        for _ in 0..ctSize {
            ctTable.push(ColorSpec::read_from(&mut reader)?);
        }
        Ok(Self {
            ccSeed,
            ccRider,
            ctSize,
            ctTable,
        })
    }
}

const FRAME_COLOR: i16 = 0;
const BODY_COLOR: i16 = 1;
const TEXT_COLOR: i16 = 2;
const ELEVATOR_COLOR: i16 = 3;
const FILL_PAT_COLOR: i16 = 4;
const ARROWS_LIGHT: i16 = 5;
const ARROWS_DARK: i16 = 6;
const THUMB_LIGHT: i16 = 7;
const THUMB_DARK: i16 = 8;
const HILITE_LIGHT: i16 = 9;
const HILITE_DARK: i16 = 10;
const TITLE_BAR_LIGHT: i16 = 11;
const TITLE_BAR_DARK: i16 = 12;
const TINGE_LIGHT: i16 = 13;
const TINGE_DARK: i16 = 14;

pub fn get_entry_name(res: &Resource) -> String {
    format!("ControlColorTable/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let clut = CtlCTab::read_from(data)?;
    for entry in clut.ctTable {
        match entry.value {
            FRAME_COLOR => write!(&mut writer, "cFrameColor")?,
            BODY_COLOR => write!(&mut writer, "cBodyColor")?,
            TEXT_COLOR => write!(&mut writer, "cTextColor")?,
            ELEVATOR_COLOR => write!(&mut writer, "cElevatorColor")?,
            FILL_PAT_COLOR => write!(&mut writer, "cFillPatColor")?,
            ARROWS_LIGHT => write!(&mut writer, "cArrowsLight")?,
            ARROWS_DARK => write!(&mut writer, "cArrowsDark")?,
            THUMB_LIGHT => write!(&mut writer, "cThumbLight")?,
            THUMB_DARK => write!(&mut writer, "cThumbDark")?,
            HILITE_LIGHT => write!(&mut writer, "cHiliteLight")?,
            HILITE_DARK => write!(&mut writer, "cHiliteDark")?,
            TITLE_BAR_LIGHT => write!(&mut writer, "cTitleBarLight")?,
            TITLE_BAR_DARK => write!(&mut writer, "cTitleBarDark")?,
            TINGE_LIGHT => write!(&mut writer, "cTingeLight")?,
            TINGE_DARK => write!(&mut writer, "cTingeDark")?,
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
