use super::Rect;
use crate::rsrcfork::Resource;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct ControlTemplate {
    controlRect: Rect,
    controlValue: i16,
    controlVisible: bool,
    fill: u8,
    controlMaximum: i16,
    controlMinimum: i16,
    controlDefProcID: i16,
    controlReference: i32,
    controlTitle: String,
}

impl ControlTemplate {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            controlRect: Rect::read_from(&mut reader)?,
            controlValue: reader.read_be_i16()?,
            controlVisible: reader.read_be_u8()? != 0,
            fill: reader.read_be_u8()?,
            controlMaximum: reader.read_be_i16()?,
            controlMinimum: reader.read_be_i16()?,
            controlDefProcID: reader.read_be_i16()?,
            controlReference: reader.read_be_i32()?,
            controlTitle: super::read_pstring(&mut reader)?,
        })
    }
}

const PUSH_BUT_PROC: i16 = 0;
const CHECK_BOX_PROC: i16 = 1;
const RADIO_BUT_PROC: i16 = 2;
const PUSH_BUT_PROC_USE_W_FONT: i16 = 8;
const CHECK_BOX_PROC_USE_W_FONT: i16 = 9;
const RADIO_BUT_PROC_USE_W_FONT: i16 = 10;
const SCROLL_BAR_PROC: i16 = 16;
const POPUP_MENU_CDEF_PROC: i16 = 1008;

pub fn get_entry_name(res: &Resource) -> String {
    format!("ControlTemplate/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let template = ControlTemplate::read_from(data)?;
    let proc_name = match template.controlDefProcID {
        PUSH_BUT_PROC => String::from("pushButProc"),
        CHECK_BOX_PROC => String::from("checkBoxProc"),
        RADIO_BUT_PROC => String::from("radioButProc"),
        PUSH_BUT_PROC_USE_W_FONT => String::from("pushButProcUseWFont"),
        CHECK_BOX_PROC_USE_W_FONT => String::from("checkBoxProcUseWFont"),
        RADIO_BUT_PROC_USE_W_FONT => String::from("radioButProcUseWFont"),
        SCROLL_BAR_PROC => String::from("scrollBarProc"),
        POPUP_MENU_CDEF_PROC => String::from("popupMenuCDEFProc"),
        _ => {
            let cdef_res_id = template.controlDefProcID / 16;
            let variation_code = template.controlDefProcID % 16;
            format!("[CDEF = {}, variation = {}]", cdef_res_id, variation_code)
        }
    };
    writeln!(
        writer,
        "ControlTemplate {{\n\
         \trect = {{ left = {left}, top = {top}, width = {width}, height = {height} }}\n\
         \tvalue = {value}\n\
         \tvisible = {visible}\n\
         \tmaximum = {maximum}\n\
         \tminimum = {minimum}\n\
         \tprocID = {procid}\n\
         \trefCon = {refcon}\n\
         \ttitle = {title:?}\n\
         }}",
        left = template.controlRect.left,
        top = template.controlRect.top,
        width = template.controlRect.width(),
        height = template.controlRect.height(),
        value = template.controlValue,
        visible = template.controlVisible,
        maximum = template.controlMaximum,
        minimum = template.controlMinimum,
        procid = proc_name,
        refcon = template.controlReference,
        title = template.controlTitle,
    )
}
