use crate::mace;
use crate::utils::{ReadExt, WriteExt};
use std::io::{self, Error, ErrorKind, Read, Write};

// NOTE: 'snd ' resources are very complex. This module does just
// enough work to decode the sound resources usually used in Glider PRO
// houses and Glider PRO's own sounds and music. It is not a general
// translator to a modern format. (That goes for all modules in this
// extractor, really.)

const stdSH: u8 = 0x00;
const extSH: u8 = 0xFF;
const cmpSH: u8 = 0xFE;

const notCompressed: i16 = 0;
const threeToOne: i16 = 3;
const sixToOne: i16 = 4;

const sixToOnePacketSize: u16 = 8;
const threeToOnePacketSize: u16 = 16;

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

struct ExtSoundHeader {
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
    instrumentChunks: u32,
    AESRecording: u32,
    sampleSize: u16,
    futureUse1: u16,
    futureUse2: u32,
    futureUse3: u32,
    futureUse4: u32,
    sampleArea: Vec<u8>,
}

fn read_ten_bytes(mut reader: impl Read) -> io::Result<[u8; 10]> {
    let mut buffer = [0; 10];
    reader.read_exact(&mut buffer).map(|_| buffer)
}

impl ExtSoundHeader {
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
            instrumentChunks: reader.read_be_u32()?,
            AESRecording: reader.read_be_u32()?,
            sampleSize: reader.read_be_u16()?,
            futureUse1: reader.read_be_u16()?,
            futureUse2: reader.read_be_u32()?,
            futureUse3: reader.read_be_u32()?,
            futureUse4: reader.read_be_u32()?,
            sampleArea: Vec::new(),
        };
        match this.sampleSize {
            8 | 16 => {}
            _ => {
                return Err(Error::new(
                    ErrorKind::InvalidData,
                    "unimplemented sample size",
                ));
            }
        }
        let frameSize = this.numChannels * u32::from(this.sampleSize) / 8;
        let sampleAreaSize = this.numFrames * frameSize;
        this.sampleArea = vec![0x00; sampleAreaSize as usize];
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
                "unimplemented channel count",
            ));
        }
        match this.compressionID {
            notCompressed | threeToOne | sixToOne => {}
            _ => {
                return Err(Error::new(
                    ErrorKind::InvalidData,
                    format!("unimplemented compression type {}", this.compressionID),
                ));
            }
        }
        match this.packetSize {
            0 => {
                this.packetSize = match this.compressionID {
                    notCompressed => 0,
                    threeToOne => threeToOnePacketSize,
                    sixToOne => sixToOnePacketSize,
                    _ => unreachable!(),
                };
            }
            threeToOnePacketSize => {
                if this.compressionID != threeToOne {
                    return Err(Error::new(
                        ErrorKind::InvalidData,
                        "invalid compression packet size",
                    ));
                }
            }
            sixToOnePacketSize => {
                if this.compressionID != sixToOne {
                    return Err(Error::new(
                        ErrorKind::InvalidData,
                        "invalid compression packet size",
                    ));
                }
            }
            _ => {
                return Err(Error::new(
                    ErrorKind::InvalidData,
                    "invalid compression packet size",
                ));
            }
        }
        match this.sampleSize {
            8 | 16 => {}
            _ => {
                return Err(Error::new(
                    ErrorKind::InvalidData,
                    "unimplemented sample size",
                ));
            }
        }
        let frameSize = match this.compressionID {
            notCompressed => this.numChannels * u32::from(this.sampleSize) / 8,
            threeToOne | sixToOne => this.numChannels * u32::from(this.packetSize) / 8,
            _ => unreachable!(),
        };
        let sampleAreaSize = this.numFrames * frameSize;
        this.sampleArea = vec![0x00; sampleAreaSize as usize];
        reader.read_exact(&mut this.sampleArea)?;
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

// Get the sound data bytes in a format compatible with WAVE files.
//
// Right now, the only change that needs to happen is to byte-swap
// 16-bit samples from big-endian to little-endian.
fn get_wave_data_bytes(sample_size: u16, sample_bytes: &[u8]) -> Vec<u8> {
    if sample_size == 16 {
        let mut new_bytes = Vec::with_capacity(sample_bytes.len());
        for word in sample_bytes.chunks_exact(2) {
            let hi_byte = word[0];
            let lo_byte = word[1];
            new_bytes.push(lo_byte);
            new_bytes.push(hi_byte);
        }
        new_bytes
    } else {
        sample_bytes.to_vec()
    }
}

fn sound_header_to_wave_data(header: &SoundHeader) -> WaveData {
    WaveData {
        channels: 1,
        sample_rate: header.sampleRate / 65536,
        bits_per_sample: 8,
        data_bytes: header.sampleArea.clone(),
    }
}

fn ext_sound_header_to_wave_data(header: &ExtSoundHeader) -> WaveData {
    WaveData {
        channels: header.numChannels as _,
        sample_rate: header.sampleRate / 65536,
        bits_per_sample: header.sampleSize,
        data_bytes: get_wave_data_bytes(header.sampleSize, &header.sampleArea),
    }
}

fn cmp_sound_header_to_wave_data(header: &CmpSoundHeader) -> io::Result<WaveData> {
    // FFmpeg's MACE decoder expands the sounds out to 16-bit samples,
    // with the high and low bytes equal. This is reduced back down to
    // 8-bit samples (even if the original sound used 16-bit samples),
    // in order to save some space.

    match header.compressionID {
        notCompressed => Ok(WaveData {
            channels: 1,
            sample_rate: header.sampleRate / 65536,
            bits_per_sample: header.sampleSize,
            data_bytes: get_wave_data_bytes(header.sampleSize, &header.sampleArea),
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
    if (resource.modifiers.len() != 1)
        || (resource.modifiers[0].modNumber != sampledSynth)
        || (resource.commands.len() != 1)
        || (resource.commands[0].cmd != dataOffsetFlag | bufferCmd)
        || (resource.commands[0].param1 != 0)
        || (resource.commands[0].param2 != 20)
    {
        return Err(io::Error::new(
            ErrorKind::InvalidData,
            "unsupported sound header",
        ));
    }
    match resource.data[20] {
        stdSH => {
            let header = SoundHeader::read_from(resource.data.as_slice())?;
            let wave_data = sound_header_to_wave_data(&header);
            write_wave_file(&wave_data, writer)
        }
        extSH => {
            let header = ExtSoundHeader::read_from(resource.data.as_slice())?;
            let wave_data = ext_sound_header_to_wave_data(&header);
            write_wave_file(&wave_data, writer)
        }
        cmpSH => {
            let header = CmpSoundHeader::read_from(resource.data.as_slice())?;
            let wave_data = cmp_sound_header_to_wave_data(&header)?;
            write_wave_file(&wave_data, writer)
        }
        _ => {
            return Err(io::Error::new(
                ErrorKind::InvalidData,
                "unknown sound header type",
            ));
        }
    }
}
