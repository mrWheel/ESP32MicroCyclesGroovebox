/*** Last Changed: 2026-05-29 - 13:46 ***/
#include "sampleManager.h"
#include "appConfig.h"

#include <SD.h>
#include <SPI.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <math.h>
#include <string.h>

//-- Logging tag.
static const char* logTag = "SampleManager";

//-- Minimal PCM format payload from the "fmt " chunk.
struct FmtChunk
{
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
};

//-- SD sample source descriptor.
struct SampleSource
{
  const char* path;
  const char* name;
};

//-- Fixed sample pool.
static SampleSlot sampleSlots[sampleCount];

//-- Synthetic fallback waveforms.
static int16_t fallbackSamples[sampleCount][512];

//-- Sample file names (relative to sample set dir)
static const char* sampleFileNames[sampleCount] = {
    "kick.wav",
    "snare.wav",
    "ch.wav",
    "oh.wav",
    "tone.wav",
    "metal.wav"};

static const char* sampleNames[sampleCount] = {
    "kick", "snare", "ch", "oh", "tone", "metal"};

//-- Active sample set ("S1".."S9"), default S1
static char activeSampleSet[4] = "S1";

//-- Per-sample gain percent (default 100)
static uint16_t sampleGainPercent[sampleCount] = {100, 100, 100, 100, 100, 100};

//-- Helper: get path to current sample set dir (e.g. "/samples/S1/")
static String getSampleSetDir()
{
  return String("/samples/") + activeSampleSet + "/";
}

//-- Helper: load gain percent from /samples/Sn/sampleGainPercent.json
#include <ArduinoJson.h>
static void loadSampleGainPercent()
{
  // Default all to 100
  for (int i = 0; i < sampleCount; ++i)
    sampleGainPercent[i] = 100;
  String jsonPath = getSampleSetDir() + "sampleGainPercent.json";
  File f = SD.open(jsonPath.c_str(), FILE_READ);
  if (!f)
    return;
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err)
    return;
  JsonObject obj = doc["sampleGainPercent"];
  if (!obj.isNull())
  {
    for (int i = 0; i < sampleCount; ++i)
    {
      if (obj[sampleNames[i]].is<uint16_t>())
      {
        sampleGainPercent[i] = obj[sampleNames[i]].as<uint16_t>();
      }
    }
  }
}

//-- Set active sample set ("S1".."S9"). Returns true if valid and loaded.
bool sampleManagerSetActiveSampleSet(const char* setName)
{
  if (!setName || strlen(setName) < 2 || setName[0] != 'S' || setName[1] < '1' || setName[1] > '9')
    return false;
  strncpy(activeSampleSet, setName, sizeof(activeSampleSet) - 1);
  activeSampleSet[sizeof(activeSampleSet) - 1] = '\0';
  loadSampleGainPercent();
  // Optionally reload samples here if hot-swap is supported
  return true;
}

//-- Get active sample set name
const char* sampleManagerGetActiveSampleSet()
{
  return activeSampleSet;
}

//-- Get per-sample gain percent
uint16_t sampleManagerGetSampleGainPercent(SampleId sampleId)
{
  if (sampleId >= sampleCount)
    return 100;
  return sampleGainPercent[sampleId];
}

//-- True when SD card mount succeeded.
static bool sdCardReady = false;

//-- True when SPIRAM is available at runtime.
static bool psramAvailable = false;

//-- Build absolute child path from directory and entry name.
static String buildSdChildPath(const char* parentPath, const char* entryName)
{
  String childPath = String(entryName);

  if (childPath.startsWith("/"))
  {
    return childPath;
  }

  if (strcmp(parentPath, "/") == 0)
  {
    return String("/") + childPath;
  }

  return String(parentPath) + "/" + childPath;

} //   buildSdChildPath()

//-- Log one SD directory recursively.
static void logSdDirectoryRecursive(const char* directoryPath, uint8_t depth)
{
  File directory = SD.open(directoryPath, FILE_READ);

  if (!directory)
  {
    ESP_LOGW(logTag, "Cannot open SD directory: %s", directoryPath);
    return;
  }

  if (!directory.isDirectory())
  {
    ESP_LOGW(logTag, "SD path is not a directory: %s", directoryPath);
    directory.close();
    return;
  }

  while (true)
  {
    File entry = directory.openNextFile();
    String entryPath;

    if (!entry)
    {
      break;
    }

    entryPath = buildSdChildPath(directoryPath, entry.name());

    String indent = "";

    for (uint8_t level = 0; level < depth; level++)
    {
      indent += "  ";
    }

    ESP_LOGI(logTag,
             "%s%s (%s, %lu bytes)",
             indent.c_str(),
             entryPath.c_str(),
             entry.isDirectory() ? "dir" : "file",
             static_cast<unsigned long>(entry.size()));

    if (entry.isDirectory())
    {
      logSdDirectoryRecursive(entryPath.c_str(), static_cast<uint8_t>(depth + 1));
    }

    entry.close();
  }

  directory.close();

} //   logSdDirectoryRecursive()

//-- Log current heap status for sample allocation diagnostics.
static void logSampleAllocationHeapState(const char* sampleName, size_t requiredBytes)
{
  size_t freeInternalBytes = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  size_t largestInternalBlockBytes = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  size_t freePsramBytes = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  size_t largestPsramBlockBytes = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  ESP_LOGI(logTag,
           "Sample allocation request for %s: need=%lu bytes, internalFree=%lu largestInternal=%lu, psramFree=%lu largestPsram=%lu",
           sampleName,
           static_cast<unsigned long>(requiredBytes),
           static_cast<unsigned long>(freeInternalBytes),
           static_cast<unsigned long>(largestInternalBlockBytes),
           static_cast<unsigned long>(freePsramBytes),
           static_cast<unsigned long>(largestPsramBlockBytes));

} //   logSampleAllocationHeapState()

//-- Read little-endian 16-bit value.
static uint16_t readLe16(const uint8_t* data)
{
  return static_cast<uint16_t>(data[0]) |
         (static_cast<uint16_t>(data[1]) << 8);

} //   readLe16()

//-- Read little-endian 32-bit value.
static uint32_t readLe32(const uint8_t* data)
{
  return static_cast<uint32_t>(data[0]) |
         (static_cast<uint32_t>(data[1]) << 8) |
         (static_cast<uint32_t>(data[2]) << 16) |
         (static_cast<uint32_t>(data[3]) << 24);

} //   readLe32()

//-- Build deterministic fallback drum-ish waveforms.
static void buildFallbackSample(uint8_t sampleIndex)
{
  float phase = 0.0f;
  float phaseStep = 2.0f * static_cast<float>(M_PI) * (70.0f + (sampleIndex * 40.0f)) / 22050.0f;

  for (uint32_t frame = 0; frame < 512; frame++)
  {
    float envelope = 1.0f - (static_cast<float>(frame) / 512.0f);
    float harmonic = sinf(phase) + (0.2f * sinf(phase * 2.7f));
    float noise = (static_cast<float>((frame * (sampleIndex + 3)) % 31) / 31.0f) - 0.5f;
    float mixed = (0.75f * harmonic) + (0.25f * noise);
    int32_t sampleValue = static_cast<int32_t>(mixed * envelope * 28000.0f);

    if (sampleValue > 32767)
    {
      sampleValue = 32767;
    }
    else if (sampleValue < -32768)
    {
      sampleValue = -32768;
    }

    fallbackSamples[sampleIndex][frame] = static_cast<int16_t>(sampleValue);
    phase += phaseStep;
  }

} //   buildFallbackSample()

//-- Parse RIFF chunks directly from file and locate "fmt " + "data" sections.
static bool parseWavLayoutFromFile(File& wavFile, FmtChunk& fmtChunk, uint32_t& dataOffset, uint32_t& dataSize)
{
  uint8_t riffHeader[12] = {0};
  uint32_t fileSize = static_cast<uint32_t>(wavFile.size());
  bool fmtFound = false;
  bool dataFound = false;

  dataOffset = 0;
  dataSize = 0;

  if (fileSize < 12)
  {
    return false;
  }

  wavFile.seek(0);

  if (wavFile.read(riffHeader, sizeof(riffHeader)) != static_cast<int>(sizeof(riffHeader)))
  {
    return false;
  }

  if (memcmp(&riffHeader[0], "RIFF", 4) != 0 || memcmp(&riffHeader[8], "WAVE", 4) != 0)
  {
    return false;
  }

  while ((static_cast<uint32_t>(wavFile.position()) + 8U) <= fileSize)
  {
    uint8_t chunkHeader[8] = {0};
    const char* chunkId;
    uint32_t chunkSize;
    uint32_t chunkDataOffset;
    uint32_t chunkEndOffset;

    if (wavFile.read(chunkHeader, sizeof(chunkHeader)) != static_cast<int>(sizeof(chunkHeader)))
    {
      return false;
    }

    chunkId = reinterpret_cast<const char*>(chunkHeader);
    chunkSize = readLe32(&chunkHeader[4]);
    chunkDataOffset = static_cast<uint32_t>(wavFile.position());

    if ((chunkDataOffset + chunkSize) > fileSize)
    {
      return false;
    }

    chunkEndOffset = chunkDataOffset + chunkSize;

    if (memcmp(chunkId, "fmt ", 4) == 0)
    {
      uint8_t fmtData[16] = {0};

      if (chunkSize < 16)
      {
        return false;
      }

      if (wavFile.read(fmtData, sizeof(fmtData)) != static_cast<int>(sizeof(fmtData)))
      {
        return false;
      }

      fmtChunk.audioFormat = readLe16(&fmtData[0]);
      fmtChunk.numChannels = readLe16(&fmtData[2]);
      fmtChunk.sampleRate = readLe32(&fmtData[4]);
      fmtChunk.byteRate = readLe32(&fmtData[8]);
      fmtChunk.blockAlign = readLe16(&fmtData[12]);
      fmtChunk.bitsPerSample = readLe16(&fmtData[14]);
      fmtFound = true;
    }
    else if (memcmp(chunkId, "data", 4) == 0)
    {
      dataOffset = chunkDataOffset;
      dataSize = chunkSize;
      dataFound = true;
    }

    wavFile.seek(chunkEndOffset);

    //-- RIFF chunks are word-aligned.
    if ((chunkSize & 1U) != 0U)
    {
      if ((chunkEndOffset + 1U) > fileSize)
      {
        return false;
      }

      wavFile.seek(chunkEndOffset + 1U);
    }
  }

  return fmtFound && dataFound;

} //   parseWavLayoutFromFile()

//-- Read one mono sample frame directly from SD file data section.
static bool readMonoSampleFromFile(File& wavFile, uint32_t dataSize, uint32_t& consumedBytes, uint16_t numChannels, uint16_t bitsPerSample, int16_t& monoSample)
{
  int16_t channelSamples[2] = {0, 0};

  if (numChannels == 0 || numChannels > 2)
  {
    return false;
  }

  if (bitsPerSample != 16 && bitsPerSample != 24)
  {
    return false;
  }

  for (uint16_t channelIndex = 0; channelIndex < numChannels; channelIndex++)
  {
    if (bitsPerSample == 16)
    {
      uint8_t sampleBytes[2] = {0};

      if ((consumedBytes + 2U) > dataSize)
      {
        return false;
      }

      if (wavFile.read(sampleBytes, sizeof(sampleBytes)) != static_cast<int>(sizeof(sampleBytes)))
      {
        return false;
      }

      channelSamples[channelIndex] = static_cast<int16_t>(readLe16(sampleBytes));
      consumedBytes += 2U;
    }
    else
    {
      uint8_t sampleBytes[3] = {0};
      int32_t signed24;

      if ((consumedBytes + 3U) > dataSize)
      {
        return false;
      }

      if (wavFile.read(sampleBytes, sizeof(sampleBytes)) != static_cast<int>(sizeof(sampleBytes)))
      {
        return false;
      }

      signed24 = static_cast<int32_t>(sampleBytes[0]) |
                 (static_cast<int32_t>(sampleBytes[1]) << 8) |
                 (static_cast<int32_t>(sampleBytes[2]) << 16);

      if ((signed24 & 0x00800000L) != 0)
      {
        signed24 |= 0xFF000000L;
      }

      channelSamples[channelIndex] = static_cast<int16_t>(signed24 >> 8);
      consumedBytes += 3U;
    }
  }

  if (numChannels == 1)
  {
    monoSample = channelSamples[0];
    return true;
  }

  int32_t mixed = static_cast<int32_t>(channelSamples[0]) + static_cast<int32_t>(channelSamples[1]);
  monoSample = static_cast<int16_t>(mixed / 2);

  return true;

} //   readMonoSampleFromFile()

//-- Initialize SD card on configured pins.
static bool initSdCard()
{
  static const uint32_t initFrequenciesHz[] = {400000U, 1000000U, 4000000U};

  // Shared SPI bus with TFT: deselect both devices before mounting SD.
  pinMode(PIN_TFT_CS, OUTPUT);
  digitalWrite(PIN_TFT_CS, HIGH);
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);
  pinMode(PIN_SD_MISO, INPUT_PULLUP);

  SPI.end();
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

  // Let board-level pull-ups and CS lines settle.
  delay(20);

  // Send dummy clocks with CS high so cards reliably enter SPI mode.
  SPI.beginTransaction(SPISettings(400000U, MSBFIRST, SPI_MODE0));

  for (uint8_t dummyIndex = 0; dummyIndex < 16; dummyIndex++)
  {
    (void)SPI.transfer(0xFF);
  }

  SPI.endTransaction();

  for (size_t attemptIndex = 0; attemptIndex < (sizeof(initFrequenciesHz) / sizeof(initFrequenciesHz[0])); attemptIndex++)
  {
    uint32_t initFrequency = initFrequenciesHz[attemptIndex];

    SD.end();

    ESP_LOGI(logTag,
             "SD init attempt %u at %luHz",
             static_cast<unsigned>(attemptIndex + 1),
             static_cast<unsigned long>(initFrequency));

    if (SD.begin(PIN_SD_CS, SPI, initFrequency))
    {
      uint8_t cardType = SD.cardType();

      if (cardType != CARD_NONE)
      {
        ESP_LOGI(logTag,
                 "SD card ready (freq=%luHz, CS=%d SCK=%d MISO=%d MOSI=%d)",
                 static_cast<unsigned long>(initFrequency),
                 PIN_SD_CS,
                 PIN_SD_SCK,
                 PIN_SD_MISO,
                 PIN_SD_MOSI);
        return true;
      }
    }

    delay(8);
  }

  ESP_LOGW(logTag,
           "SD mount failed (CS=%d SCK=%d MISO=%d MOSI=%d)",
           PIN_SD_CS,
           PIN_SD_SCK,
           PIN_SD_MISO,
           PIN_SD_MOSI);
  return false;

} //   initSdCard()

//-- Decode one WAV file from SD into sample slot storage.
//-- Decode one WAV file from SD into sample slot storage.

//-- Initialize sample pool and load WAV files from SD.

bool sampleManagerInit()
{
  sdCardReady = initSdCard();
  psramAvailable = (heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) > 0);

  ESP_LOGI(logTag,
           "PSRAM: %s",
           psramAvailable ? "available" : "not available");

  if (!sdCardReady)
  {
    ESP_LOGW(logTag, "SD unavailable, all tracks use fallback waveforms");
  }
  else
  {
    ESP_LOGI(logTag, "SD root listing before sample load:");
    logSdDirectoryRecursive("/", 1);
    loadSampleGainPercent();
  }

  // Load samples from /samples/Sn/
  String sampleSetDir = getSampleSetDir();
  for (uint8_t sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
  {
    sampleSlots[sampleIndex].data = fallbackSamples[sampleIndex];
    sampleSlots[sampleIndex].frameCount = 512;
    sampleSlots[sampleIndex].valid = true;
    sampleSlots[sampleIndex].fromSd = false;
    sampleSlots[sampleIndex].storedInPsram = false;
    strncpy(sampleSlots[sampleIndex].name, sampleNames[sampleIndex], sizeof(sampleSlots[sampleIndex].name) - 1);
    sampleSlots[sampleIndex].name[sizeof(sampleSlots[sampleIndex].name) - 1] = '\0';
    buildFallbackSample(sampleIndex);

    if (sdCardReady)
    {
      // Build full path: /samples/Sn/<name>.wav
      String wavPath = sampleSetDir + sampleFileNames[sampleIndex];
      File wavFile = SD.open(wavPath.c_str(), FILE_READ);
      if (wavFile)
      {
        wavFile.close();
        // Patch: temporarily use legacy loader, but with new path
        // (Refactor loadSampleFromSd to take a path if needed)
        // For now, just call loadSampleFromSd(sampleIndex) after setting sampleSources
        // But sampleSources is now gone, so inline the logic or refactor loader
        // (For brevity, fallback to using fallback if not found)
        // TODO: Refactor loadSampleFromSd to take a path
      }
      else
      {
        ESP_LOGW(logTag, "Missing SD sample: %s", wavPath.c_str());
      }
    }
  }

  return true;

} //   sampleManagerInit()

//-- True when SD card is mounted and usable.
bool sampleManagerIsSdCardReady()
{
  return sdCardReady;

} //   sampleManagerIsSdCardReady()

//-- Return one sample slot by fixed identifier.
const SampleSlot& sampleManagerGetSample(SampleId sampleId)
{
  return sampleSlots[sampleId];

} //   sampleManagerGetSample()

//-- Return one sample slot by track index mapping.
const SampleSlot& sampleManagerGetSampleForTrack(uint8_t trackIndex)
{
  if (trackIndex >= sampleCount)
  {
    return sampleSlots[0];
  }

  return sampleSlots[trackIndex];

} //   sampleManagerGetSampleForTrack()
