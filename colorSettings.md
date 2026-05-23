[← Back to README](README.md)

# Color Settings — ultimateTimer Display

## Hardware background

This project uses an ST7789-based 2.4-inch SPI TFT panel.
This specific panel has **hardware color inversion active (INVON)**.
Every pixel value you write is bit-complemented by the panel before it is shown.

## The PANEL_COLOR macro

```cpp
#define PANEL_COLOR(colorValue) static_cast<uint16_t>((colorValue) ^ 0xFFFFU)
```

Use this macro whenever you want a **colored fill or colored text** to appear in the color
you specify.

- Pass the **desired visual color** (RGB565 value you want to SEE on screen).
- The macro pre-inverts it.
- The panel inverts it back, so the visual result matches what you passed in.

**Example:**
```cpp
// Header background that actually looks blue on screen.
static const uint16_t headerBackgroundColor = PANEL_COLOR(0x001F);  // 0x001F = blue (RGB565)
```

## Text colors — NEVER use PANEL_COLOR for text

Because the panel inverts all values, the two Adafruit constants behave counter-intuitively:

| Constant | Written value | Panel inverts to | Appears on screen as |
|---|---|---|---|
| `ST77XX_BLACK` (0x0000) | 0x0000 | 0xFFFF | **WHITE** (use for legible text) |
| `ST77XX_WHITE` (0xFFFF) | 0xFFFF | 0x0000 | **BLACK** (invisible on dark backgrounds) |

**Rule:** Always use `ST77XX_BLACK` for text that must be readable.
Never use `ST77XX_WHITE` for text — it renders black and disappears on dark backgrounds.

## Screen background

`tft.fillScreen(ST77XX_BLACK)` produces a **black screen**.
All UI elements are drawn on top of this black background.

## Proven color constants (do not change without hardware test)

```cpp
// Header and selected item fill — visually blue
static const uint16_t selectionFillColor    = PANEL_COLOR(0x001F);

// Selected item border — visually light blue/teal
static const uint16_t selectionBorderColor  = PANEL_COLOR(0x7DFF);

// Selected item text — visually white (high-contrast on blue)
static const uint16_t selectionTextColor    = ST77XX_BLACK;

// Idle (unselected) button fill — visually light gray
static const uint16_t idleButtonFillColor   = PANEL_COLOR(0xD69A);

// Idle button border — visually lighter gray
static const uint16_t idleButtonBorderColor = PANEL_COLOR(0xBDF7);

// All button text — visually white on both blue and gray backgrounds
static const uint16_t selectedButtonTextColor = ST77XX_BLACK;
static const uint16_t idleButtonTextColor     = ST77XX_BLACK;
```

## How to add a new colored UI element

1. Pick the **desired RGB565 color** (use any RGB565 color picker tool).
2. Wrap it with `PANEL_COLOR(your_value)` for fills, borders, and colored text.
3. Use `ST77XX_BLACK` for text that must appear white/light on a colored background.
4. Never use `ST77XX_WHITE` for text — it will be invisible on dark backgrounds.
5. Test with the `TEST_COLOR_PATERN` build flag if in doubt.

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| `tileValueColor = ST77XX_WHITE` | Text invisible on dark tile | Change to `ST77XX_BLACK` |
| Fill color `PANEL_COLOR(0x2104)` (near-black) | Tile invisible on black background | Use `selectionFillColor` or a clearly different hue |
| Using raw 0xRRGG for fills without `PANEL_COLOR` | Color looks wrong/inverted | Wrap with `PANEL_COLOR()` |

## Button rendering — one rule for ALL buttons

Every button in this project uses the **same** dark/light convention, regardless of context:

| State | Fill | Border | Text |
|---|---|---|---|
| **Selected / active** | `getUiSelectedFillColor()` = `PANEL_COLOR(getDarkColor())` | `getUiSelectedBorderColor()` | `getUiSelectedTextColor()` |
| **Unselected / inactive** | `getUiInactiveFillColor()` = `PANEL_COLOR(getLightColor())` | `getUiInactiveBorderColor()` | `getUiInactiveTextColor()` |

Additional rule: the **selected button** always gets an extra inner border drawn with `ST77XX_BLACK`.
On this inverted panel `ST77XX_BLACK` (0x0000) → panel inverts → appears **WHITE**, making the inner border visible.

```cpp
// Every button, both Timer Screen and Field Input:
uint16_t buttonFill   = isSelected ? getUiSelectedFillColor()   : getUiInactiveFillColor();
uint16_t buttonBorder = isSelected ? getUiSelectedBorderColor() : getUiInactiveBorderColor();
uint16_t buttonText   = isSelected ? getUiSelectedTextColor()   : getUiInactiveTextColor();

tft.fillRoundRect(x, y, w, h, 8, buttonFill);
tft.drawRoundRect(x, y, w, h, 8, buttonBorder);
if (isSelected) {
  tft.drawRoundRect(x+1, y+1, w-2, h-2, 8, ST77XX_BLACK);  // inner border → appears WHITE
}
tft.setTextColor(buttonText, buttonFill);
```

This matches the visual appearance of the Timer Screen Start/Stop/Reset buttons exactly.
Do NOT invert this logic for any button type.

### Practical UI rule (must stay true)

- Timer Screen action buttons and Field Input selection buttons must look the same:
  - selected = dark fill + selected border + white inner border + selected text
  - unselected = light fill + inactive border + inactive text
- Any future button UI should reuse this same mapping to avoid confusing behavior.
- In button-mode Field Input (`positionCount == 1`, `tokenCount 2..6`):
  - buttons are rendered on the black background (no tile panel behind them)
  - 2-4 options are centered in one row
  - 5-6 options are centered in two rows, with the lower row placed near the bottom of the screen
  - rotate encoder = change selected button
  - encoder SHORT/MEDIUM/LONG press = apply selected value and return
  - PIN_KEY0 SHORT = back without save, PIN_KEY0 MEDIUM/LONG = save and return

## Color profile rules (active project convention)

Color profiles are defined in `include/colorSettings.h` and use this format:

`ColorName: darkLevel-darkLabelColor, lightLevel-lightLabelColor`

- The first number is the dark/active variant index for that color.
- The second number is the light/inactive/background variant index for that color.
- Label color after each number defines the visual text color for that variant (`WHITE` or `BLACK`).
- `Orange` is currently marked as not used in palette workflows.

Current configured profiles:

- `Red: 1-WHITE, 6-WHITE`
- `Green: 1-BLACK, 5-BLACK`
- `Blue: 1-WHITE, 6-WHITE`
- `Indigo: 1-WHITE, 6-WHITE`
- `Violet: 1-WHITE, 7-WHITE`
- `Yellow: 1-BLACK, 5-BLACK`
- `Orange: Not used`

## Request interpretation defaults (from now on)

- If asked: `text must be red`, use the **first** (dark) variant of `Red`.
- If asked: `tile must be yellow`, use the **second** (light/background) variant of `Yellow`.
- For **all** buttons: dark shade = selected/active, light shade = idle/inactive. No exceptions.

## Visual text mapping for this inverted panel

- Visual `WHITE` text -> use `ST77XX_BLACK`.
- Visual `BLACK` text -> use `ST77XX_WHITE`.

[← Back to README](README.md)
