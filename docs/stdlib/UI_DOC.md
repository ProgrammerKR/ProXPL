# ProXPL UI Standard Library (UI Kit)

The `UI` module provides a comprehensive set of design tokens and utility strings for building modern, high-performance web interfaces in ProXPL.

## Core Concepts

ProXPL UI is built on a **Token-Based Design System**. Instead of writing raw CSS, you use predefined constants that represent design decisions (colors, spacing, shadows, etc.).

### Usage
```javascript
use UI;

App MyView {
    Main(style: PROX_THEME_MIDNIGHT) {
        Container(style: GLASS_CARD + " " + ANIM_FADE_IN) {
            Text(style: TEXT_XL + " " + FONT_BOLD) { "Hello ProXPL" }
        }
    }
}
```

---

## 🎨 Design Tokens

### Colors
- `COLOR_PRIMARY`: Core brand color (#3b82f6)
- `COLOR_SUCCESS`: Positive actions (#22c55e)
- `COLOR_DANGER`: Destructive actions (#ef4444)
- `COLOR_GLASS`: Transparent white for glass effects

### Premium Gradients v2
| Token | Visual Style |
|-------|--------------|
| `GRADIENT_VIOLET_DREAM` | Deep purple to magenta |
| `GRADIENT_CYBER_PUNK` | Neon blue to vivid cyan |
| `GRADIENT_ROSE_GOLD` | Soft rose to elegant gold |

---

## 🏗️ Layout Utilities

### Bento Grid
The **Bento Grid** is a modern layout pattern for creating responsive, tiled interfaces.

- `BENTO_GRID`: Initializer for a 4-column auto-row grid.
- `BENTO_ITEM_W2`: Span 2 columns.
- `BENTO_ITEM_H2`: Span 2 rows.

**Example:**
```javascript
Section(style: BENTO_GRID) {
    Container(style: GLASS_CARD + BENTO_ITEM_W2 + BENTO_ITEM_H2) { "Main Feature" }
    Container(style: GLASS_CARD) { "Metric 1" }
    Container(style: GLASS_CARD) { "Metric 2" }
}
```

---

## 🪟 Components

### Glassmorphism v2
Refined glass effects with high saturation and backdrop blurring.
- `GLASS_V2_LIGHT`: Premium light glass.
- `GLASS_V2_DARK`: Premium dark glass.
- `GLASS_V2_PURPLE`: Themed purple glass.

### Stat Cards
- `STAT_CARD`: Container for metric displays.
- `STAT_VALUE`: Large, prominent font for numbers.
- `STAT_LABEL`: Small, muted label.

---

## 🎬 Animations
- `ANIM_FADE_IN`: Smooth opacity entry.
- `ANIM_ZOOM_IN`: Scale and opacity entry.
- `ANIM_BOUNCE`: Attention-grabbing bounce.
- `ANIM_SKELETON`: Shimmer effect for loading states.

---

## 🛠️ Build Commands
Build your UI to a web-ready format using the `prm` tool:

```bash
prm build web                   # Build to default dist/
prm build web --output myapp    # Build to custom folder
```
