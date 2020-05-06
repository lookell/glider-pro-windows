use super::Rect;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct AlertTemplate {
    boundsRect: Rect,
    itemsID: i16,
    stages: StageList,
}

struct StageList([Stage; 4]);

struct Stage {
    bold_button: bool,
    visible: bool,
    beeps: u8,
}

impl AlertTemplate {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            boundsRect: Rect::read_from(&mut reader)?,
            itemsID: reader.read_be_i16()?,
            stages: StageList::read_from(&mut reader)?,
        })
    }
}

impl StageList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let packed = reader.read_be_u16()?;
        Ok(Self([
            Stage::from_nibble((packed >> 12) as _),
            Stage::from_nibble((packed >> 8) as _),
            Stage::from_nibble((packed >> 4) as _),
            Stage::from_nibble(packed as _),
        ]))
    }
}

impl Stage {
    fn from_nibble(nibble: u8) -> Self {
        Self {
            bold_button: nibble & 0x8 != 0,
            visible: nibble & 0x4 != 0,
            beeps: nibble & 0x3,
        }
    }
}

const OK: bool = false;
const Cancel: bool = true;
const silent: u8 = 0;
const sound1: u8 = 1;
const sound2: u8 = 2;
const sound3: u8 = 3;

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    fn bold_to_str(bold: bool) -> &'static str {
        match bold {
            OK => "OK",
            Cancel => "Cancel",
        }
    }

    fn sound_to_str(sound: u8) -> &'static str {
        match sound {
            silent => "silent",
            sound1 => "sound1",
            sound2 => "sound2",
            sound3 => "sound3",
            _ => unreachable!(),
        }
    }

    let template = AlertTemplate::read_from(data)?;
    writeln!(
        &mut writer,
        "# dlu({}, {}, {}, {})",
        super::pix_to_xdlu(template.boundsRect.left.into()),
        super::pix_to_ydlu(template.boundsRect.top.into()),
        super::pix_to_xdlu(template.boundsRect.width()),
        super::pix_to_ydlu(template.boundsRect.height()),
    )?;
    writeln!(
        writer,
        "Alert {{\n\
         \tboundsRect = {{ left = {left}, top = {top}, width = {width}, height = {height} }}\n\
         \titemsID = {itemsID}\n\
         \tstages = [\n\
         \t\t{{ boldOutline = {bold_1}, visible = {visible_1}, sound = {sound_1} }}\n\
         \t\t{{ boldOutline = {bold_2}, visible = {visible_2}, sound = {sound_2} }}\n\
         \t\t{{ boldOutline = {bold_3}, visible = {visible_3}, sound = {sound_3} }}\n\
         \t\t{{ boldOutline = {bold_4}, visible = {visible_4}, sound = {sound_4} }}\n\
         \t]\n\
         }}",
        left = template.boundsRect.left,
        top = template.boundsRect.top,
        width = template.boundsRect.width(),
        height = template.boundsRect.height(),
        itemsID = template.itemsID,
        bold_1 = bold_to_str(template.stages.0[0].bold_button),
        visible_1 = template.stages.0[0].visible,
        sound_1 = sound_to_str(template.stages.0[0].beeps),
        bold_2 = bold_to_str(template.stages.0[1].bold_button),
        visible_2 = template.stages.0[1].visible,
        sound_2 = sound_to_str(template.stages.0[1].beeps),
        bold_3 = bold_to_str(template.stages.0[2].bold_button),
        visible_3 = template.stages.0[2].visible,
        sound_3 = sound_to_str(template.stages.0[2].beeps),
        bold_4 = bold_to_str(template.stages.0[3].bold_button),
        visible_4 = template.stages.0[3].visible,
        sound_4 = sound_to_str(template.stages.0[3].beeps),
    )
}
