use crate::utils::{ReadExt, SeekExt};
use std::io::{self, Read, Seek};

// See https://files.stairways.com/other/macbinaryii-standard-info.txt for format details.

#[allow(unused)]
struct HeaderRaw {
    version: u8,
    file_name: [u8; 64],
    file_type: u32,
    file_creator: u32,
    original_finder_flags: u8,
    zero_1: u8,
    vpos: u16,
    hpos: u16,
    id: u16,
    protected: u8,
    zero_2: u8,
    data_length: u32,
    rsrc_length: u32,
    creation_date: u32,
    modified_date: u32,
    get_info_length: u16,
    new_finder_flags: u8,
    _ignored: u32,
    secondary_header_length: u16,
    upload_version: u8,
    reader_version: u8,
    _crc: u16,
}

impl HeaderRaw {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            version: reader.read_be_u8()?,
            file_name: {
                let mut buf = [0; 64];
                reader.read_exact(&mut buf).map(|_| buf)?
            },
            file_type: reader.read_be_u32()?,
            file_creator: reader.read_be_u32()?,
            original_finder_flags: reader.read_be_u8()?,
            zero_1: reader.read_be_u8()?,
            vpos: reader.read_be_u16()?,
            hpos: reader.read_be_u16()?,
            id: reader.read_be_u16()?,
            protected: reader.read_be_u8()?,
            zero_2: reader.read_be_u8()?,
            data_length: reader.read_be_u32()?,
            rsrc_length: reader.read_be_u32()?,
            creation_date: reader.read_be_u32()?,
            modified_date: reader.read_be_u32()?,
            get_info_length: reader.read_be_u16()?,
            new_finder_flags: reader.read_be_u8()?,
            _ignored: reader.read_be_u32()?,
            secondary_header_length: reader.read_be_u16()?,
            upload_version: reader.read_be_u8()?,
            reader_version: reader.read_be_u8()?,
            _crc: reader.read_be_u16()?,
        })
    }
}

pub struct MacBinary {
    pub data: Vec<u8>,
    pub rsrc: Vec<u8>,
}

impl MacBinary {
    pub fn read_from(mut reader: impl Read + Seek) -> io::Result<Option<Self>> {
        let header = HeaderRaw::read_from(&mut reader)?;
        if header.version != 0 {
            return Ok(None);
        }
        if header.file_name[0] > 63 {
            return Ok(None);
        }
        if header.zero_1 != 0 || header.zero_2 != 0 {
            return Ok(None);
        }
        let mut secondary = vec![0; header.secondary_header_length.into()];
        reader.read_exact(secondary.as_mut_slice())?;
        reader.align_to(128)?;
        let mut data = vec![0; header.data_length as _];
        reader.read_exact(data.as_mut_slice())?;
        reader.align_to(128)?;
        let mut rsrc = vec![0; header.rsrc_length as _];
        reader.read_exact(rsrc.as_mut_slice())?;
        reader.align_to(128)?;
        Ok(Some(Self { data, rsrc }))
    }
}
