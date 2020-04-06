use super::Rect;
use crate::mac_roman;
use crate::rsrcfork::Resource;
use crate::utils::{ReadExt, SeekExt};
use std::io::{self, Cursor, Read, Seek, Write};

struct ItemList {
    items: Vec<Item>,
}

impl ItemList {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Self> {
        let num_items = reader.read_be_i16()?.wrapping_add(1);
        let mut items = Vec::with_capacity((num_items.max(0) as u16).into());
        for _ in 0..num_items {
            items.push(Item::read_from(&mut reader)?);
        }
        Ok(Self { items })
    }
}

struct Item {
    reserved: i32,
    bounds: Rect,
    disabled: bool,
    data: ItemData,
}

impl Item {
    fn read_from(mut reader: impl Read + Seek) -> io::Result<Self> {
        let reserved = reader.read_be_i32()?;
        let bounds = Rect::read_from(&mut reader)?;
        let flags = reader.read_be_u8()?;
        let disabled = (flags & 0x80) != 0;
        let item_type = flags & 0x7F;
        let data = ItemData::read_from(&mut reader, item_type)?;
        Ok(Self {
            reserved,
            bounds,
            disabled,
            data,
        })
    }
}

const Button: u8 = 4;
const CheckBox: u8 = 5;
const RadioButton: u8 = 6;
const Control: u8 = 7;
const StaticText: u8 = 8;
const EditText: u8 = 16;
const Icon: u8 = 32;
const Picture: u8 = 64;
const UserItem: u8 = 0;

enum ItemData {
    Button(String),
    CheckBox(String),
    RadioButton(String),
    Control(i16),
    StaticText(String),
    EditText(String),
    Icon(i16),
    Picture(i16),
    UserItem,
    UnknownItem(Vec<u8>),
}

impl ItemData {
    fn read_from(mut reader: impl Read + Seek, item_type: u8) -> io::Result<Self> {
        let length = reader.read_be_u8()?;
        let mut data = vec![0; length.into()];
        reader.read_exact(&mut data)?;
        reader.align_to(2)?;
        let text_string = data
            .iter()
            .copied()
            .map(mac_roman::decode)
            .collect::<String>();
        Ok(match item_type {
            Button => Self::Button(text_string),
            CheckBox => Self::CheckBox(text_string),
            RadioButton => Self::RadioButton(text_string),
            Control => Self::Control(data.as_slice().read_be_i16()?),
            StaticText => Self::StaticText(text_string),
            EditText => Self::EditText(text_string),
            Icon => Self::Icon(data.as_slice().read_be_i16()?),
            Picture => Self::Picture(data.as_slice().read_be_i16()?),
            UserItem => Self::UserItem,
            _ => Self::UnknownItem(data),
        })
    }
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("ItemList/{}.txt", res.id)
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let item_list = ItemList::read_from(Cursor::new(data))?;
    writeln!(&mut writer, "Dialog [")?;
    for item in item_list.items.into_iter() {
        writeln!(
            &mut writer,
            "\t# dlu({}, {}, {}, {})",
            super::pix_to_xdlu(item.bounds.left.into()),
            super::pix_to_ydlu(item.bounds.top.into()),
            super::pix_to_xdlu(item.bounds.width()),
            super::pix_to_ydlu(item.bounds.height()),
        )?;
        write!(&mut writer, "\t{{ ")?;
        match item.data {
            ItemData::Button(text) => write!(&mut writer, "Button({:?})", text)?,
            ItemData::CheckBox(text) => write!(&mut writer, "CheckBox({:?})", text)?,
            ItemData::RadioButton(text) => write!(&mut writer, "RadioButton({:?})", text)?,
            ItemData::Control(id) => write!(&mut writer, "Control({})", id)?,
            ItemData::StaticText(text) => write!(&mut writer, "StaticText({:?})", text)?,
            ItemData::EditText(text) => write!(&mut writer, "EditText({:?})", text)?,
            ItemData::Icon(id) => write!(&mut writer, "Icon({:?})", id)?,
            ItemData::Picture(id) => write!(&mut writer, "Picture({:?})", id)?,
            ItemData::UserItem => write!(&mut writer, "UserItem()")?,
            ItemData::UnknownItem(_) => write!(&mut writer, "UnknownItem(????)")?,
        }
        if item.disabled {
            write!(&mut writer, ", disabled")?;
        } else {
            write!(&mut writer, ", enabled")?;
        }
        write!(
            &mut writer,
            ", x = {}, y = {}, ",
            item.bounds.left, item.bounds.top
        )?;
        write!(
            &mut writer,
            "w = {}, h = {} ",
            item.bounds.width(),
            item.bounds.height()
        )?;
        writeln!(&mut writer, "}}")?;
    }
    writeln!(&mut writer, "]")?;
    Ok(())
}
