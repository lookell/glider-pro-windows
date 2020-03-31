use crate::rsrcfork::Resource;
use crate::utils::{ReadExt, WriteExt};
use std::io::{self, Read, Write};

// NOTE: 'snd ' resources are very complex. This module does just
// enough work to decode the sound resources usually used in Glider PRO
// houses and Glider PRO's own sounds and music. It is not a general
// translator to a modern format. (That goes for all modules in this
// extractor, really.)

const stdSH: u8 = 0x00;
const cmpSH: u8 = 0xFE;

const fixedCompression: i16 = -1;
const notCompressed: i16 = 0;
const threeToOne: i16 = 3;
const sixToOne: i16 = 4;

const sampledSynth: u16 = 5;

const dataOffsetFlag: u16 = 0x8000;
const bufferCmd: u16 = 81;

struct SoundHeader {
    samplePtr: u32,
    length: u32,
    sampleRate: u32,
    loopStart: u32,
    loopEnd: u32,
    encode: u8,
    baseFrequency: u8,
    sampleArea: Vec<u8>,
}

impl SoundHeader {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut this = Self {
            samplePtr: reader.read_be_u32()?,
            length: reader.read_be_u32()?,
            sampleRate: reader.read_be_u32()?,
            loopStart: reader.read_be_u32()?,
            loopEnd: reader.read_be_u32()?,
            encode: reader.read_be_u8()?,
            baseFrequency: reader.read_be_u8()?,
            sampleArea: Vec::new(),
        };
        this.sampleArea = vec![0x00; this.length as usize];
        reader.read_exact(&mut this.sampleArea)?;
        Ok(this)
    }
}

struct CmpSoundHeader {
    samplePtr: u32,
    numChannels: u32,
    sampleRate: u32,
    loopStart: u32,
    loopEnd: u32,
    encode: u8,
    baseFrequency: u8,
    numFrames: u32,
    AIFFSampleRate: [u8; 10],
    markerChunk: u32,
    format: [u8; 4],
    futureUse2: u32,
    stateVars: u32,
    leftOverSamples: u32,
    compressionID: i16,
    packetSize: u16,
    snthID: u16,
    sampleSize: u16,
    sampleArea: Vec<u8>,
}

fn read_four_bytes(mut reader: impl Read) -> io::Result<[u8; 4]> {
    let mut buffer = [0; 4];
    reader.read_exact(&mut buffer).map(|_| buffer)
}

fn read_ten_bytes(mut reader: impl Read) -> io::Result<[u8; 10]> {
    let mut buffer = [0; 10];
    reader.read_exact(&mut buffer).map(|_| buffer)
}

impl CmpSoundHeader {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let mut this = Self {
            samplePtr: reader.read_be_u32()?,
            numChannels: reader.read_be_u32()?,
            sampleRate: reader.read_be_u32()?,
            loopStart: reader.read_be_u32()?,
            loopEnd: reader.read_be_u32()?,
            encode: reader.read_be_u8()?,
            baseFrequency: reader.read_be_u8()?,
            numFrames: reader.read_be_u32()?,
            AIFFSampleRate: read_ten_bytes(&mut reader)?,
            markerChunk: reader.read_be_u32()?,
            format: read_four_bytes(&mut reader)?,
            futureUse2: reader.read_be_u32()?,
            stateVars: reader.read_be_u32()?,
            leftOverSamples: reader.read_be_u32()?,
            compressionID: reader.read_be_i16()?,
            packetSize: reader.read_be_u16()?,
            snthID: reader.read_be_u16()?,
            sampleSize: reader.read_be_u16()?,
            sampleArea: Vec::new(),
        };
        reader.read_to_end(&mut this.sampleArea)?;
        Ok(this)
    }
}

struct ModRef {
    modNumber: u16,
    modInit: i32,
}

impl ModRef {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            modNumber: reader.read_be_u16()?,
            modInit: reader.read_be_i32()?,
        })
    }
}

struct SndCommand {
    cmd: u16,
    param1: i16,
    param2: i32,
}

impl SndCommand {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        Ok(Self {
            cmd: reader.read_be_u16()?,
            param1: reader.read_be_i16()?,
            param2: reader.read_be_i32()?,
        })
    }
}

struct SndListResource {
    format: i16,
    modifiers: Vec<ModRef>,
    commands: Vec<SndCommand>,
    data: Vec<u8>,
}

impl SndListResource {
    fn read_from(mut reader: impl Read) -> io::Result<Self> {
        let format = reader.read_be_i16()?;
        assert_eq!(format, 0x0001, "format 1 sound required");
        let numModifiers = reader.read_be_u16()?;
        let mut modifiers = Vec::with_capacity(numModifiers.into());
        for _ in 0..numModifiers {
            modifiers.push(ModRef::read_from(&mut reader)?);
        }
        let numCommands = reader.read_be_u16()?;
        let mut commands = Vec::with_capacity(numCommands.into());
        for _ in 0..numCommands {
            commands.push(SndCommand::read_from(&mut reader)?);
        }
        let mut data = Vec::new();
        reader.read_to_end(&mut data)?;
        Ok(Self {
            format,
            modifiers,
            commands,
            data,
        })
    }
}

fn convert_fixed_to_float80_bytes(fixed: u32) -> [u8; 10] {
    let fixed_float = fixed as f64 / 65536.0;
    let fixed_bits = fixed_float.to_bits();
    let sign = (fixed_bits >> 63) as u8;
    let exponent = ((fixed_bits >> 52) & 0x7FF) as i16 - 1023 + 16383;
    let exponent_bytes = exponent.to_be_bytes();
    let fraction = (fixed_bits & 0xF_FFFF_FFFF_FFFF) << 11;
    let fraction_bytes = fraction.to_be_bytes();
    [
        exponent_bytes[0] | (sign << 7),
        exponent_bytes[1],
        fraction_bytes[0] | 0x80,
        fraction_bytes[1],
        fraction_bytes[2],
        fraction_bytes[3],
        fraction_bytes[4],
        fraction_bytes[5],
        fraction_bytes[6],
        fraction_bytes[7],
    ]
}

fn write_aiff_file(header: SoundHeader, mut writer: impl Write) -> io::Result<()> {
    // Form container chunk
    writer.write_all(b"FORM")?;
    writer.write_be_u32(4 + 30 + 16 + header.sampleArea.len() as u32)?;
    writer.write_all(b"AIFF")?;
    // Common chunk
    writer.write_all(b"COMM")?;
    writer.write_be_u32(22)?;
    writer.write_be_u16(1)?;
    writer.write_be_u32(header.sampleArea.len() as _)?;
    writer.write_be_u16(8)?;
    writer.write_all(&convert_fixed_to_float80_bytes(header.sampleRate))?;
    writer.write_all(b"NONE")?;
    // Sound Data chunk
    writer.write_all(b"SSND")?;
    writer.write_be_u32(8 + header.sampleArea.len() as u32)?;
    writer.write_be_u32(0)?;
    writer.write_be_u32(0)?;
    let adjustedSamples = header
        .sampleArea
        .iter()
        .map(|byte| byte.wrapping_add(0x80))
        .collect::<Vec<_>>();
    writer.write_all(&adjustedSamples)?;
    if header.sampleArea.len() % 2 != 0 {
        writer.write_all(&[0])?; // padding
    }
    Ok(())
}

fn write_aiff_c_file(header: CmpSoundHeader, mut writer: impl Write) -> io::Result<()> {
    // Form container chunk
    writer.write_all(b"FORM")?;
    writer.write_be_u32(4 + 12 + 30 + 16 + header.sampleArea.len() as u32)?;
    writer.write_all(b"AIFC")?;
    // Format Version chunk
    writer.write_all(b"FVER")?;
    writer.write_be_u32(4)?;
    writer.write_be_u32(0xA2805140)?;
    // Common chunk
    writer.write_all(b"COMM")?;
    writer.write_be_u32(22)?;
    writer.write_be_u16(header.numChannels as _)?;
    writer.write_be_u32(header.numFrames)?;
    writer.write_be_u16(header.sampleSize)?;
    writer.write_all(&header.AIFFSampleRate)?;
    match header.compressionID {
        fixedCompression => writer.write_all(&header.format)?,
        notCompressed => writer.write_all(b"NONE")?,
        threeToOne => writer.write_all(b"MAC3")?,
        sixToOne => writer.write_all(b"MAC6")?,
        _ => unimplemented!("sound compression ID {}", header.compressionID),
    }
    // Sound Data chunk
    writer.write_all(b"SSND")?;
    writer.write_be_u32(8 + header.sampleArea.len() as u32)?;
    writer.write_be_u32(0)?;
    writer.write_be_u32(0)?;
    writer.write_all(&header.sampleArea)?;
    if header.sampleArea.len() % 2 != 0 {
        writer.write_all(&[0])?; // padding
    }
    Ok(())
}

pub fn get_entry_name(res: &Resource) -> String {
    format!("Sound/{}.aif", res.id)
}

pub fn convert(data: &[u8], writer: impl Write) -> io::Result<()> {
    let resource = SndListResource::read_from(data)?;
    assert_eq!(resource.modifiers.len(), 1);
    assert_eq!(resource.modifiers[0].modNumber, sampledSynth);
    assert_eq!(resource.commands.len(), 1);
    assert_eq!(resource.commands[0].cmd, dataOffsetFlag | bufferCmd);
    assert_eq!(resource.commands[0].param1, 0);
    assert_eq!(resource.commands[0].param2, 20);
    match resource.data[20] {
        stdSH => {
            let header = SoundHeader::read_from(resource.data.as_slice())?;
            write_aiff_file(header, writer)
        }
        cmpSH => {
            let header = CmpSoundHeader::read_from(resource.data.as_slice())?;
            write_aiff_c_file(header, writer)
        }
        _ => unimplemented!("unknown sound header type"),
    }
}
