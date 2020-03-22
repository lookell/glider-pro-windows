use super::Rect;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct Window {
    boundsRect: Rect,
    procID: i16,
    visible: bool,
    filler1: u8,
    goAwayFlag: bool,
    filler2: u8,
    refCon: i32,
    title: String,
}

impl Window {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            boundsRect: Rect::read_from(&mut reader)?,
            procID: reader.read_be_i16()?,
            visible: reader.read_be_u8()? != 0,
            filler1: reader.read_be_u8()?,
            goAwayFlag: reader.read_be_u8()? != 0,
            filler2: reader.read_be_u8()?,
            refCon: reader.read_be_i32()?,
            title: super::read_pstring(&mut reader)?,
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Window/{}.txt", res.id)
}

const documentProc: i16 = 0;
const dBoxProc: i16 = 1;
const plainDBox: i16 = 2;
const altDBoxProc: i16 = 3;
const noGrowDocProc: i16 = 4;
const movableDBoxProc: i16 = 5;
const zoomDocProc: i16 = 8;
const zoomNoGrow: i16 = 12;
const rDocProc: i16 = 16;

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let template = Window::read_from(data)?;
    let proc_name = match template.procID {
        documentProc => String::from("documentProc"),
        dBoxProc => String::from("dBoxProc"),
        plainDBox => String::from("plainDBox"),
        altDBoxProc => String::from("altDBoxProc"),
        noGrowDocProc => String::from("noGrowDocProc"),
        movableDBoxProc => String::from("movableDBoxProc"),
        zoomDocProc => String::from("zoomDocProc"),
        zoomNoGrow => String::from("zoomNoGrow"),
        rDocProc => String::from("rDocProc"),
        _ => format!("[procID #{}]", template.procID),
    };
    writeln!(&mut writer, "Window {{")?;
    writeln!(&mut writer, "\tboundsRect = {{ x = {}, y = {}, w = {}, h = {} }}", template.boundsRect.left, template.boundsRect.top, template.boundsRect.width(), template.boundsRect.height())?;
    writeln!(&mut writer, "\tprocID = {}", proc_name)?;
    writeln!(&mut writer, "\tvisible = {}", template.visible)?;
    writeln!(&mut writer, "\tgoAwayFlag = {}", template.goAwayFlag)?;
    writeln!(&mut writer, "\trefCon = {}", template.refCon)?;
    writeln!(&mut writer, "\ttitle = {:?}", template.title)?;
    writeln!(&mut writer, "}}")?;
    Ok(())
}
