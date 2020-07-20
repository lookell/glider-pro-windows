use crate::mace;
use crate::utils::{ReadExt, WriteExt};
use std::io::{self, Error, ErrorKind, Read, Write};

// NOTE: 'snd ' resources are very complex. This module does just
// enough work to decode the sound resources usually used in Glider PRO
// houses and Glider PRO's own sounds and music. It is not a general
// translator to a modern format. (That goes for all modules in this
// extractor, really.)

const stdSH: u8 = 0x00;
const cmpSH: u8 = 0xFE;

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
        if this.numChannels != 1 {
            return Err(Error::new(
                ErrorKind::InvalidData,
                "unimplement channel count",
            ));
        }
        if this.sampleSize != 8 {
            return Err(Error::new(
                ErrorKind::InvalidData,
                "unimplemented sample size",
            ));
        }
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
        if format != 0x0001 {
            return Err(Error::new(
                ErrorKind::InvalidData,
                "format 1 sound required",
            ));
        }
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

struct WaveData {
    channels: u16,
    sample_rate: u32,
    bits_per_sample: u16,
    data_bytes: Vec<u8>,
}

fn write_wave_file(wave: &WaveData, mut writer: impl Write) -> io::Result<()> {
    const WAVE_FORMAT_PCM: u16 = 1;
    let block_align = wave.channels * wave.bits_per_sample / 8;
    let avg_bytes_per_sec = wave.sample_rate * u32::from(block_align);
    let mut padded_wave_length = wave.data_bytes.len() as u32;
    if (padded_wave_length % 2) != 0 {
        padded_wave_length += 1; // pad to WORD boundary
    }
    // 'RIFF' container chunk
    writer.write_all(b"RIFF")?;
    writer.write_le_u32(4 + 24 + 8 + padded_wave_length)?;
    writer.write_all(b"WAVE")?;
    // 'fmt ' chunk
    writer.write_all(b"fmt ")?;
    writer.write_le_u32(16)?;
    writer.write_le_u16(1)?; // WAVE_FORMAT_PCM
    writer.write_le_u16(wave.channels)?;
    writer.write_le_u32(wave.sample_rate)?;
    writer.write_le_u32(avg_bytes_per_sec)?;
    writer.write_le_u16(block_align)?;
    writer.write_le_u16(wave.bits_per_sample)?;
    // 'data' chunk
    writer.write_all(b"data")?;
    writer.write_le_u32(padded_wave_length)?;
    writer.write_all(&wave.data_bytes)?;
    if (wave.data_bytes.len() % 2) != 0 {
        writer.write_all(&[0])?; // pad to WORD boundary
    }
    Ok(())
}

fn sound_header_to_wave_data(header: &SoundHeader) -> WaveData {
    WaveData {
        channels: 1,
        sample_rate: header.sampleRate / 65536,
        bits_per_sample: 8,
        data_bytes: header.sampleArea.clone(),
    }
}

fn cmp_sound_header_to_wave_data(header: &CmpSoundHeader) -> io::Result<WaveData> {
    // FFmpeg's MACE decoder expands the sounds out to 16-bit samples,
    // with the high and low bytes equal. This is reduced back down to
    // 8-bit samples, to save some space.

    match header.compressionID {
        notCompressed => Ok(WaveData {
            channels: 1,
            sample_rate: header.sampleRate / 65536,
            bits_per_sample: 8,
            data_bytes: header.sampleArea.clone(),
        }),
        threeToOne => {
            let data_bytes = mace::mace3_decode_mono(&header.sampleArea)?
                .into_iter()
                .map(|n| ((n as u16) / 256 + 128) as u8)
                .collect::<Vec<u8>>();
            Ok(WaveData {
                channels: 1,
                sample_rate: header.sampleRate / 65536,
                bits_per_sample: 8,
                data_bytes,
            })
        }
        sixToOne => {
            let data_bytes = mace::mace6_decode_mono(&header.sampleArea)?
                .into_iter()
                .map(|n| ((n as u16) / 256 + 128) as u8)
                .collect::<Vec<u8>>();
            Ok(WaveData {
                channels: 1,
                sample_rate: header.sampleRate / 65536,
                bits_per_sample: 8,
                data_bytes,
            })
        }
        _ => {
            return Err(Error::new(
                ErrorKind::InvalidData,
                "unknown sound compression type",
            ))
        }
    }
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
            let wave_data = sound_header_to_wave_data(&header);
            write_wave_file(&wave_data, writer)
        }
        cmpSH => {
            let header = CmpSoundHeader::read_from(resource.data.as_slice())?;
            let wave_data = cmp_sound_header_to_wave_data(&header)?;
            write_wave_file(&wave_data, writer)
        }
        _ => unimplemented!("unknown sound header type"),
    }
}
