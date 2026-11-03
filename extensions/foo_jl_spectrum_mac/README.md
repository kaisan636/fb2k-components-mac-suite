# foo_jl_spectrum

> Part of [foobar2000 macOS Components Suite](../../README.md)

**[Features & Documentation](../../docs/spectrum.md)** | **[Changelog](CHANGELOG.md)**

---

A real-time spectrum analyzer UI element for foobar2000 macOS. Reads PCM from the core `visualisation_manager` stream, runs its own FFT (Apple vDSP) and renders with Core Graphics.

Contributed by [Scannou](https://github.com/Scannou).

## Requirements

- foobar2000 for Mac 2.x
- macOS 12.0+ (Monterey or later)
- Xcode 15+ with Command Line Tools (for building from source)
- Ruby (for project generation)

## Quick Start

### From Binary (this fork: prebuilt via CI)

This fork builds the component on GitHub Actions (no local Xcode needed):

1. Open the repo's **Actions → Build Spectrum Analyzer (macOS)**, run it with
   **Run workflow** (or just push; it also runs automatically when this
   extension's sources change).
2. Download the artifact **`foo_jl_spectrum.fb2k-component`** from the run.
3. Install and toggle the SPL readout:

```bash
unzip foo_jl_spectrum.fb2k-component
xattr -dr com.apple.quarantine mac/foo_jl_spectrum.component
mkdir -p ~/Library/foobar2000-v2/user-components/foo_jl_spectrum
cp -R mac/foo_jl_spectrum.component ~/Library/foobar2000-v2/user-components/foo_jl_spectrum/
# restart foobar2000, add the panel (View → Layout → Edit Layout, line `spectrum`),
# then right-click the panel → "Show SPL Readout"
```

The attached `.fb2k-component` is the same signed-package layout the upstream
releases use (a zip with `mac/foo_jl_spectrum.component` inside); the strip of
the quarantine attribute is still needed because CI builds are unsigned.

### Build and Install (local, needs Xcode)

```bash
# From this directory
./Scripts/build.sh --install            # Build Release and install
./Scripts/build.sh --clean --install    # Clean rebuild and install
./Scripts/build.sh --regenerate --install  # After adding/removing source files

# Restart foobar2000, then add the UI element
```

### From Binary

1. Download `foo_jl_spectrum.fb2k-component` from [Releases](https://github.com/JendaT/fb2k-components-mac-suite/releases?q=spectrum)
2. In foobar2000, open **Preferences** (⌘,) **→ Components**, click **+** and choose the file
3. Restart foobar2000 when it asks

## Build Scripts

All scripts are located in the `Scripts/` directory and share `shared/scripts/lib.sh`.

| Script | Purpose |
|--------|---------|
| `build.sh` | Build (`--debug`, `--release`, `--clean`, `--regenerate`, `--install`) |
| `install.sh` | Install a pre-built component (`--config Debug`) |
| `clean.sh` | Remove build artifacts |
| `generate_xcode_project.rb` | Generate the Xcode project from `src/` |

Never call `xcodebuild` directly; `build.sh` keeps output in the local `build/` directory so `install.sh` copies the binary you just built.

## Usage

### Adding to Layout

Open **View → Layout → Edit Layout...**, add a line with `spectrum` inside a splitter where you want the panel, and click **Apply**.

Layout names: `spectrum` (also `spectrum_analyzer`, `Spectrum Analyzer`, `foo_jl_spectrum`, `jl_spectrum`).

### Configuration

Access settings in **Preferences > Display > Spectrum Analyzer**, or right-click the element and choose **Preferences...**.

See [docs/spectrum.md](../../docs/spectrum.md) for the full settings reference.

### SPL Readout (this fork)

Right-click the panel → **Show SPL Readout** to draw one line at the top-left:

```
SPL 71.3 dB · MAX 83.2 dB (−2.1 dBFS PK · −12.4 dBFS RMS)
```

- **SPL** is the broadband RMS loudness and **MAX** the highest true peak of
  the current song (resets on track change, on stop, or via right-click →
  "Reset SPL Peak"), converted with:
  `SPL = calibration offset + level dBFS + playback volume dB`
- The offset defaults to 96.0 dB SPL at 0 dBFS (headphone sensitivity 96 dB
  SPL, digital full scale at 1.0 V). Tune it with the config-store key
  `spl_offset_tenths` (0.1 dB steps, e.g. 960) after measuring your setup.
- The meter only reads the visualisation stream; playback audio is untouched.

## Project Structure

```
foo_jl_spectrum_mac/
├── src/
│   ├── Core/
│   │   ├── SpectrumAnalyzer.h/cpp   # FFT, band mapping, smoothing, dynamics (pure C++)
│   │   ├── SpectrumConfig.h         # Config keys, enums, defaults, configStore accessors
│   │   └── SpectrumThemes.h         # Color theme presets
│   ├── UI/
│   │   ├── SpectrumView.h/mm        # Core Graphics rendering (bars, curve, grids)
│   │   ├── SpectrumController.h/mm  # View controller, display link / timer, glass background, quit handling
│   │   └── SpectrumPreferences.h/mm # Preferences page
│   ├── Integration/
│   │   └── Main.mm                  # UI element and component registration
│   ├── fb2k_sdk.h                   # SDK configuration
│   └── Prefix.pch                   # Precompiled header
├── Resources/
│   └── Info.plist
├── Scripts/
└── README.md
```

## Technical Details

- Audio comes from `visualisation_stream_v2::get_chunk_absolute` (all channels); the component windows it (Hann or Blackman-Harris) and runs a vDSP real FFT.
- Updates come from the view's `CADisplayLink` on macOS 14+ and a 60 fps `NSTimer` on macOS 12-13; they pause when the view is hidden and the stream is released so the core can stop the visualisation backend.
- An `initquit` service releases all live streams before service teardown; holding a stream through shutdown aborts the app.
- Display window is set by Top and Range (default 0 dB to -90 dB); bars and grid share the settings so guides line up.
- Settings persist through `fb2k::configStore` under the `foo_jl_spectrum.` prefix.

## License

MIT License - see LICENSE file
