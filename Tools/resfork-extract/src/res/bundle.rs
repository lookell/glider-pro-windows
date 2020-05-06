use crate::rsrcfork::ResType;
use crate::utils::ReadExt;
use std::io::{self, Read, Write};

#[allow(dead_code)]
struct Bundle {
    signature: ResType,
    version: i16,
    type_map: Vec<TypeMapping>,
}

impl Bundle {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let signature = ResType::read_from(&mut reader)?;
        let version = reader.read_be_i16()?;
        let num_types = reader.read_be_u16()?.wrapping_add(1);
        let mut type_map = Vec::with_capacity(num_types.into());
        for _ in 0..num_types {
            type_map.push(TypeMapping::read_from(&mut reader)?);
        }
        Ok(Self {
            signature,
            version,
            type_map,
        })
    }
}

struct TypeMapping {
    restype: ResType,
    id_map: Vec<IDMapping>,
}

impl TypeMapping {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let restype = ResType::read_from(&mut reader)?;
        let num_mappings = reader.read_be_u16()?.wrapping_add(1);
        let mut id_map = Vec::with_capacity(num_mappings.into());
        for _ in 0..num_mappings {
            id_map.push(IDMapping::read_from(&mut reader)?);
        }
        Ok(Self { restype, id_map })
    }
}

struct IDMapping {
    local_id: i16,
    actual_id: i16,
}

impl IDMapping {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            local_id: reader.read_be_i16()?,
            actual_id: reader.read_be_i16()?,
        })
    }
}

pub fn convert(data: &[u8], mut writer: impl Write) -> io::Result<()> {
    let bundle = Bundle::read_from(data)?;
    writeln!(&mut writer, "signature: '{}'", bundle.signature)?;
    writeln!(&mut writer, "mappings:")?;
    for type_mapping in &bundle.type_map {
        writeln!(&mut writer, "\t'{}'", type_mapping.restype)?;
        for id_mapping in &type_mapping.id_map {
            let local = id_mapping.local_id;
            let actual = id_mapping.actual_id;
            writeln!(&mut writer, "\t\tlocal({}) = actual({})", local, actual)?;
        }
    }
    Ok(())
}
