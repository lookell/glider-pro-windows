use crate::utils::ReadExt;
use std::io::{self, Read, Seek, SeekFrom};

// See
// https://web.archive.org/web/20180311140826/http://kaiser-edv.de/documents/AppleSingle_AppleDouble.pdf
// for format details.

const SINGLE_MAGIC: u32 = 0x0005_1600;
const DOUBLE_MAGIC: u32 = 0x0005_1607;
const VERSION_ONE: u32 = 0x0001_0000;
const VERSION_TWO: u32 = 0x0002_0000;
const DATA_ENTRY_ID: u32 = 1;
const RSRC_ENTRY_ID: u32 = 2;

struct HeaderRaw {
    magic: u32,
    version: u32,
    _filler: [u8; 16],
    entries_count: u16,
}

impl HeaderRaw {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            magic: reader.read_be_u32()?,
            version: reader.read_be_u32()?,
            _filler: {
                let mut buf = [0; 16];
                reader.read_exact(&mut buf).map(|_| buf)?
            },
            entries_count: reader.read_be_u16()?,
        })
    }
}

struct EntryRaw {
    id: u32,
    offset: u32,
    length: u32,
}

impl EntryRaw {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            id: reader.read_be_u32()?,
            offset: reader.read_be_u32()?,
            length: reader.read_be_u32()?,
        })
    }
}

#[allow(unused)]
pub struct AppleDouble {
    pub data: Vec<u8>,
    pub rsrc: Vec<u8>,
}

impl AppleDouble {
    pub fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        let header = HeaderRaw::read_from(&mut reader)?;
        if header.magic != SINGLE_MAGIC && header.magic != DOUBLE_MAGIC {
            return Ok(None);
        }
        if header.version != VERSION_ONE && header.version != VERSION_TWO {
            return Ok(None);
        }
        let mut data_entry = None;
        let mut rsrc_entry = None;
        for _ in 0..header.entries_count {
            let entry = EntryRaw::read_from(&mut reader)?;
            match entry.id {
                DATA_ENTRY_ID => data_entry = Some(entry),
                RSRC_ENTRY_ID => rsrc_entry = Some(entry),
                _ => {}
            }
        }
        let mut data = Vec::new();
        if let Some(data_entry) = data_entry {
            data.resize(data_entry.length as _, 0);
            reader.seek(SeekFrom::Start(data_entry.offset.into()))?;
            reader.read_exact(data.as_mut_slice())?;
        }
        let mut rsrc = Vec::new();
        if let Some(rsrc_entry) = rsrc_entry {
            rsrc.resize(rsrc_entry.length as _, 0);
            reader.seek(SeekFrom::Start(rsrc_entry.offset.into()))?;
            reader.read_exact(rsrc.as_mut_slice())?;
        }
        Ok(Some(Self { data, rsrc }))
    }
}
