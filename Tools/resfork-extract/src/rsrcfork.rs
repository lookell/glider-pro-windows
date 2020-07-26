#![allow(dead_code)]

use crate::mac_roman;
use crate::utils::ReadExt;
use std::fmt::{self, Debug, Display, Formatter};
use std::io::{self, Cursor, Read, Seek, SeekFrom};
use std::slice;
use std::vec;

#[derive(Clone, Copy, Eq, PartialEq)]
pub struct ResType {
    pub bytes: [u8; 4],
}

impl Display for ResType {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        let string: String = self.bytes.iter().map(|&b| mac_roman::decode(b)).collect();
        write!(f, "{}", string)
    }
}

impl Debug for ResType {
    fn fmt(&self, f: &mut Formatter) -> fmt::Result {
        write!(f, "{:?}", self.to_string())
    }
}

impl From<&'_ [u8; 4]> for ResType {
    fn from(src: &'_ [u8; 4]) -> Self {
        Self::new(src)
    }
}

impl ResType {
    pub fn new(bytes: &[u8; 4]) -> Self {
        Self { bytes: *bytes }
    }

    pub fn as_bstr(&self) -> &[u8; 4] {
        &self.bytes
    }

    pub fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut bytes = [0; 4];
        reader.read_exact(&mut bytes)?;
        Ok(Self { bytes })
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
        let name = buffer.into_iter().map(mac_roman::decode).collect();
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

impl IntoIterator for ResourceFork {
    type Item = Resource;
    type IntoIter = vec::IntoIter<Resource>;

    fn into_iter(self) -> Self::IntoIter {
        self.resources.into_iter()
    }
}

impl<'a> IntoIterator for &'a ResourceFork {
    type Item = &'a Resource;
    type IntoIter = slice::Iter<'a, Resource>;

    fn into_iter(self) -> Self::IntoIter {
        self.resources.iter()
    }
}

impl<'a> IntoIterator for &'a mut ResourceFork {
    type Item = &'a mut Resource;
    type IntoIter = slice::IterMut<'a, Resource>;

    fn into_iter(self) -> Self::IntoIter {
        self.resources.iter_mut()
    }
}

impl ResourceFork {
    pub fn iter(&self) -> impl Iterator<Item = &'_ Resource> {
        self.resources.iter()
    }

    pub fn load(&self, restype: impl Into<ResType>, resid: i16) -> Option<&Resource> {
        let restype = restype.into();
        self.iter()
            .find(|res| res.restype == restype && res.id == resid)
    }

    pub fn iter_type<T>(&self, restype: T) -> impl Iterator<Item = &'_ Resource>
    where
        T: Into<ResType>,
    {
        let restype = restype.into();
        self.iter().filter(move |res| res.restype == restype)
    }

    pub fn iter_mut(&mut self) -> impl Iterator<Item = &'_ mut Resource> {
        self.resources.iter_mut()
    }

    pub fn iter_type_mut(&mut self, restype: ResType) -> impl Iterator<Item = &'_ mut Resource> {
        self.iter_mut().filter(move |res| res.restype == restype)
    }

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
