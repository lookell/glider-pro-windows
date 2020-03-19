#![allow(dead_code)]

use crate::mac_roman;
use crate::utils::ReadExt;
use std::fmt::{self, Debug, Display, Formatter};
use std::io::{self, Cursor, Read, Seek, SeekFrom};

#[derive(Clone, Copy)]
pub struct ResType {
    chars: [char; 4],
}

impl Display for ResType {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{}", self.chars.iter().collect::<String>())
    }
}

impl Debug for ResType {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{:?}", self.to_string())
    }
}

impl ResType {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut bytes = [0; 4];
        reader.read_exact(&mut bytes)?;
        Ok(Self {
            chars: [
                mac_roman::decode_byte(bytes[0]),
                mac_roman::decode_byte(bytes[1]),
                mac_roman::decode_byte(bytes[2]),
                mac_roman::decode_byte(bytes[3]),
            ],
        })
    }
}

#[derive(Debug)]
struct ForkHeader {
    data_offset: u32,
    rsrc_offset: u32,
    data_length: u32,
    rsrc_length: u32,
}

impl ForkHeader {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            data_offset: reader.read_be_u32()?,
            rsrc_offset: reader.read_be_u32()?,
            data_length: reader.read_be_u32()?,
            rsrc_length: reader.read_be_u32()?,
        })
    }
}

struct ResourceData {
    data: Vec<u8>,
}

impl ResourceData {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let data_length = reader.read_be_u32()?;
        let mut data = vec![0x00; data_length as _];
        reader.read_exact(data.as_mut_slice())?;
        Ok(Self { data })
    }
}

#[derive(Debug)]
struct MapHeader {
    reserved_header: ForkHeader,
    reserved_handle: u32,
    reserved_fileref: u16,
    attributes: u16,
    type_list_offset: u16,
    name_list_offset: u16,
}

impl MapHeader {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            reserved_header: ForkHeader::read_from(&mut reader)?,
            reserved_handle: reader.read_be_u32()?,
            reserved_fileref: reader.read_be_u16()?,
            attributes: reader.read_be_u16()?,
            type_list_offset: reader.read_be_u16()?,
            name_list_offset: reader.read_be_u16()?,
        })
    }
}

#[derive(Debug)]
struct TypeList {
    items: Vec<TypeListItem>,
}

impl TypeList {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let number_of_items = reader.read_be_u16()?.wrapping_add(1);
        let mut items = Vec::with_capacity(number_of_items.into());
        for _ in 0..number_of_items {
            items.push(TypeListItem::read_from(&mut reader)?);
        }
        Ok(Self { items })
    }
}

#[derive(Debug)]
struct TypeListItem {
    restype: ResType,
    num_resources: u16,
    references_offset: u16,
}

impl TypeListItem {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            restype: ResType::read_from(&mut reader)?,
            num_resources: reader.read_be_u16()?.wrapping_add(1),
            references_offset: reader.read_be_u16()?,
        })
    }
}

#[derive(Debug)]
struct ReferenceListItem {
    id: i16,
    name_offset: i16,
    attributes: u8,
    data_offset: u32,
    reserved_handle: u32,
}

impl ReferenceListItem {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            id: reader.read_be_i16()?,
            name_offset: reader.read_be_i16()?,
            attributes: reader.read_be_u8()?,
            data_offset: reader.read_be_u24()?, // 3-byte integer
            reserved_handle: reader.read_be_u32()?,
        })
    }
}

struct ResourceName {
    name: String,
}

impl ResourceName {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let length = reader.read_be_u8()?;
        let mut buffer = vec![0x00; length.into()];
        reader.read_exact(buffer.as_mut_slice())?;
        let name = buffer.into_iter().map(mac_roman::decode_byte).collect();
        Ok(Self { name })
    }
}

pub struct Resource {
    pub restype: ResType,
    pub id: i16,
    pub name: Option<String>,
    pub data: Vec<u8>,
}

pub struct ResourceFork {
    pub resources: Vec<Resource>,
}

impl ResourceFork {
    pub fn read_from(mut reader: impl Read + Seek) -> io::Result<Self> {
        let seek_base = reader.seek(SeekFrom::Current(0))?;
        let header = ForkHeader::read_from(&mut reader)?;

        let mut res_data_bytes = vec![0x00; header.data_length as _];
        reader.seek(SeekFrom::Start(seek_base + u64::from(header.data_offset)))?;
        reader.read_exact(res_data_bytes.as_mut_slice())?;
        let res_data_bytes = res_data_bytes;

        let mut res_map_bytes = vec![0x00; header.rsrc_length as _];
        reader.seek(SeekFrom::Start(seek_base + u64::from(header.rsrc_offset)))?;
        reader.read_exact(res_map_bytes.as_mut_slice())?;
        let mut res_map_cursor = Cursor::new(res_map_bytes);

        let header = MapHeader::read_from(&mut res_map_cursor)?;

        res_map_cursor.seek(SeekFrom::Start(header.type_list_offset.into()))?;
        let type_list = TypeList::read_from(&mut res_map_cursor)?.items;

        let mut reference_lists = Vec::new();
        for type_item in type_list.iter() {
            res_map_cursor.seek(SeekFrom::Start(header.type_list_offset.into()))?;
            res_map_cursor.seek(SeekFrom::Current(type_item.references_offset.into()))?;
            let mut ref_list = Vec::new();
            for _ in 0..type_item.num_resources {
                ref_list.push(ReferenceListItem::read_from(&mut res_map_cursor)?);
            }
            reference_lists.push(ref_list);
        }

        let mut resources = Vec::new();
        for (type_item, ref_list) in type_list.iter().zip(&reference_lists) {
            for ref_item in ref_list {
                let res_name = if ref_item.name_offset < 0 {
                    None
                } else {
                    res_map_cursor.seek(SeekFrom::Start(header.name_list_offset.into()))?;
                    res_map_cursor.seek(SeekFrom::Current(ref_item.name_offset.into()))?;
                    Some(ResourceName::read_from(&mut res_map_cursor)?.name)
                };
                let data_offset = ref_item.data_offset as usize;
                let res_data = ResourceData::read_from(&res_data_bytes[data_offset..])?.data;
                resources.push(Resource {
                    restype: type_item.restype,
                    id: ref_item.id,
                    name: res_name,
                    data: res_data,
                })
            }
        }

        Ok(Self { resources })
    }
}
