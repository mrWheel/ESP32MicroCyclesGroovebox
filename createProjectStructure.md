# createProjectStructure.py

## Purpose

`createProjectStructure.py` generates a deployable `projects/<project-name>` folder from a PlatformIO project. It builds each PlatformIO environment, collects firmware artifacts, creates metadata defaults when needed, and can optionally sync output to AWS with `rsync`.

Script version in source: `v2.1 (2026-02-28)`.

## What the Script Does

### 1. Validates input and mode

- Expects a PlatformIO project path as positional argument.
- Supports two AWS sync modes:
  - `--sync-aws`: build first, then sync only the current generated project folder.
  - `--only-sync-aws`: skip build and sync the full local `projects/` folder.
- `--sync-aws` and `--only-sync-aws` are mutually exclusive.
- If started without arguments, it prints help and exits.

### 2. In build mode (default path)

When not using `--only-sync-aws`, it performs:

- Verifies `pio` CLI is available.
- Verifies `platformio.ini` exists.
- Detects `workspace_dir` from `[platformio]` in `platformio.ini` (falls back to `.pio`).
- Parses all `[env:...]` sections.
- Automatically skips any environment whose name contains `skip` (case-insensitive, so `skip`, `SKIP`, etc.).
- Resolves per-environment board and SoC family (`esp32` or `esp8266`).
- Detects firmware version from `PROG_VERSION` in files under `src/`.
  - Looks for semantic version patterns and normalizes to `vX.Y.Z`.
  - Falls back to `v0.0.0` if not found.

### 3. Creates output folder structure

For project `<name>`, output root is:

- `projects/<name>/`

If it already exists, it is removed and recreated.

For each environment, artifacts go into versioned folders:

- If a board appears in only one env:
  - `projects/<name>/<board>/<version>/`
- If the same board is used by multiple envs:
  - `projects/<name>/<env>/<board>/<version>/`

Environments containing `skip` in their name are excluded entirely from build and folder generation.

### 4. Ensures project metadata exists

If `projectMetaData/` is missing in the source project, it creates defaults:

- `projectMetaData/project_en.md`
- `projectMetaData/project_nl.md`
- `projectMetaData/project.json`
- `projectMetaData/thisProject.png` (downloaded from a default URL; empty file if download fails)

Then all metadata files are copied to `projects/<name>/`.

### 5. Builds and collects artifacts per environment

For every PlatformIO environment:

- Runs:
  - `pio run -e <env>`
- If `data/` exists, tries:
  - `pio run -e <env> -t buildfs`
  - BuildFS errors are logged as warnings and do not stop the script.

Only non-skipped environments are processed.

Then it copies artifacts from the PlatformIO build directory:

- Required:
  - `firmware.bin` (script fails if missing)
- Optional when present:
  - `bootloader.bin`
  - `partitions.bin`
  - `boot_app0.bin`
  - `partitions.csv`
  - filesystem image (`spiffs.bin` or `littlefs.bin` / `LittleFS.bin`)

It also tries to resolve and copy:

- Partition source from `board_build.partitions` (or default `partitions.csv` for ESP32).
- ESP8266 linker script from `board_build.ldscript` or generated `.ld` in build output.

### 6. Generates `flash.json`

In each version folder, it writes `flash.json` containing:

- `board`
- `soc`
- `version`
- `flash_files`: ordered list of `{ offset, file }`

Offset rules include:

- ESP32 bootloader offset:
  - `0x1000` normally
  - `0x0000` for ESP32-S3 boards
- ESP32 partitions offset: `0x8000`
- ESP32 boot_app0 offset: `0xe000`
- Firmware offset detected from `partitions.csv` (with sensible fallbacks)
- Filesystem offset from `partitions.csv`
  - For ESP8266, if unavailable, tries `_FS_start` from linker script
  - Final ESP8266 fallback: `0x300000`

### 7. Generates build log

For each environment version directory, creates `build_log.md` with:

- executed commands
- stdout/stderr captures
- warnings
- resolved build directory path

### 8. Optional AWS sync

Defaults in script:

- Server: `admin@aandewiel.nl`
- Target base: `/home/admin/flasherWebsite_v3`
- SSH key: `~/.ssh/LightsailDefaultKey-eu-central-1.pem`

`--sync-aws`:

- Creates remote folder:
  - `<target>/projects/<project-name>`
- Uses `rsync -avz --update` to copy only new/updated files.
- Never deletes remote files.

`--only-sync-aws`:

- Validates local `projects/` first:
  - each project folder must contain `project.json`, `project_en.md`, `project_nl.md`
  - must have at least one `flash.json` with sibling `firmware.bin`
- Syncs entire local `projects/` to remote `<target>/projects/`.

Both sync modes support:

- `--aws-dry-run` to preview rsync changes without copying.

## Requirements

- Python 3.10+ (uses modern type hints like `str | None`)
- PlatformIO Core CLI (`pio`) in `PATH` for build mode
- `platformio.ini` with at least one `[env:...]`
- At least one environment name that does not contain `skip` (case-insensitive)
- SSH key file present for AWS sync
- System tools available: `rsync`, `ssh`

## Usage

From repository root (or any location using absolute/relative path):

```bash
python3 createProjectStructure.py <path-to-platformio-project>
```

Build and then sync only this project:

```bash
python3 createProjectStructure.py <path-to-platformio-project> --sync-aws
```

Preview build+sync changes (no upload):

```bash
python3 createProjectStructure.py <path-to-platformio-project> --sync-aws --aws-dry-run
```

Skip build; validate and sync complete local `projects/` folder:

```bash
python3 createProjectStructure.py <path-to-platformio-project> --only-sync-aws
```

Preview full-folder sync only:

```bash
python3 createProjectStructure.py <path-to-platformio-project> --only-sync-aws --aws-dry-run
```

## Notes and Behaviors

- The script changes into the given project directory before building.
- Existing `projects/<project-name>/` is removed before regeneration.
- Any `[env:...]` section with `skip` in its name is ignored for building and output folder creation.
- `buildfs` failures do not stop artifact generation.
- Missing `firmware.bin` for any environment is a hard error.
- In `--only-sync-aws`, no build happens; it only validates and syncs existing output.
