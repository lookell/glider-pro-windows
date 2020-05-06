use crate::mac_roman;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

struct Menu {
    menuID: i16,
    menuWidth: i16,
    menuHeight: i16,
    menuProc: u32,
    enableFlags: u32,
    menuTitle: String,
    menuItems: Vec<MenuItem>,
}

impl Menu {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let menuID = reader.read_be_i16()?;
        let menuWidth = reader.read_be_i16()?;
        let menuHeight = reader.read_be_i16()?;
        let menuProc = reader.read_be_u32()?;
        assert_eq!(menuProc, 0, "cannot interpret non-standard menu data");
        let enableFlags = reader.read_be_u32()?;
        let menuTitle = super::read_pstring(&mut reader)?;
        let mut menuItems = Vec::new();
        loop {
            menuItems.push(match MenuItem::read_from(&mut reader)? {
                Some(item) => item,
                None => break,
            });
        }
        Ok(Self {
            menuID,
            menuWidth,
            menuHeight,
            menuProc,
            enableFlags,
            menuTitle,
            menuItems,
        })
    }
}

struct MenuItem {
    itemTitle: String,
    itemData1: u8,
    itemData2: u8,
    itemData3: u8,
    itemStyle: u8,
}

impl MenuItem {
    fn read_from(mut reader: impl Read) -> io::Result<Option<Self>> {
        let itemTitle = super::read_pstring(&mut reader)?;
        if itemTitle.is_empty() {
            return Ok(None);
        }
        let itemData1 = reader.read_be_u8()?;
        let itemData2 = reader.read_be_u8()?;
        let itemData3 = reader.read_be_u8()?;
        let itemStyle = reader.read_be_u8()?;
        Ok(Some(Self {
            itemTitle,
            itemData1,
            itemData2,
            itemData3,
            itemStyle,
        }))
    }
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let menu = Menu::read_from(data)?;
    write!(
        &mut writer,
        "menu ({}, {:?}, ",
        menu.menuID, &menu.menuTitle,
    )?;
    if menu.enableFlags & 0x1 != 0 {
        writeln!(&mut writer, "enabled):")?;
    } else {
        writeln!(&mut writer, "disabled):")?;
    }
    for (i, item) in menu.menuItems.iter().enumerate() {
        let enabled = if i < 32 {
            (menu.enableFlags & (1 << (i + 1))) != 0
        } else {
            true
        };
        write!(
            &mut writer,
            "\titem ({:?}, {}, {:?}, {}, {}",
            item.itemTitle,
            item.itemData1,
            mac_roman::decode(item.itemData2),
            item.itemData3,
            item.itemStyle,
        )?;
        if enabled {
            writeln!(&mut writer, ", enabled);")?;
        } else {
            writeln!(&mut writer, ", disabled);")?;
        }
    }
    Ok(())
}
