# UI Features Documentation

## Overview

This document describes the user interface features and design principles of the Cosmopolitan web application.

## Material Design Implementation

The application follows Google's Material Design principles for a modern, intuitive user experience.

### Color Scheme

**Primary Colors:**
- Primary: `#1976d2` (Blue 700)
- Primary Dark: `#1565c0` (Blue 800)
- Accent: `#ff4081` (Pink A200)
- Text Primary: `rgba(0, 0, 0, 0.87)`
- Text Secondary: `rgba(0, 0, 0, 0.54)`

**Background Colors:**
- Body: `#f5f5f5` (Gray 100)
- Card: `#ffffff` (White)
- Header Gradient: `linear-gradient(135deg, #667eea 0%, #764ba2 100%)`

### Components

#### Cards
Material Design cards with elevation and rounded corners:
```css
.card {
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    padding: 24px;
    margin-bottom: 24px;
}
```

#### Buttons
Touch-friendly buttons with minimum 48px height:
```css
.btn {
    min-height: 48px;
    padding: 12px 24px;
    border-radius: 4px;
    font-size: 16px;
    transition: all 0.3s ease;
}
```

**Button Types:**
- Primary: Blue background (`#1976d2`)
- Secondary: Gray border with white background
- Full Width: For mobile devices

#### Forms
Modern form inputs with focus states:
```css
input, textarea {
    border: 1px solid #ddd;
    border-radius: 4px;
    padding: 12px;
    transition: all 0.3s ease;
}

input:focus, textarea:focus {
    border-color: #1976d2;
    box-shadow: 0 0 0 3px rgba(25, 118, 210, 0.1);
}
```

#### Headers
Gradient header cards with visual impact:
```css
.header-card {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    border-radius: 8px;
    padding: 32px 24px;
    color: white;
    box-shadow: 0 4px 6px rgba(0,0,0,0.1);
}
```

### Responsive Design

The UI adapts to different screen sizes using breakpoints:

**Mobile First Approach:**
- Default: Mobile devices (< 768px)
- Tablet: 768px - 1024px
- Desktop: > 1024px

**Mobile Optimizations:**
- Full-width buttons
- Single column layouts
- Larger touch targets (48px minimum)
- Simplified navigation
- Reduced padding/margins

**Example:**
```css
@media (max-width: 768px) {
    .btn {
        width: 100%;
        margin-bottom: 12px;
    }
    
    .card {
        padding: 16px;
        margin-bottom: 16px;
    }
}
```

## Kaomoji Emoticon Picker

### Overview

The Kaomoji picker provides easy access to 100+ Japanese emoticons (顔文字) for enhancing messages and posts.

### Features

**12 Categories:**
1. **Happy** (嬉しい) - `(^▽^)`, `(*^‿^*)`, `ヽ(^o^)丿`
2. **Excited** (興奮) - `(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧`, `✧*｡٩(ˊᗜˋ*)و✧*｡`
3. **Love** (愛) - `(♥ω♥)`, `(´∀｀)♡`, `(｡・//ε//・｡)`
4. **Sad** (悲しい) - `(╥﹏╥)`, `(｡•́︿•̀｡)`, `(ಥ﹏ಥ)`
5. **Angry** (怒り) - `(╬ Ò﹏Ó)`, `ヽ(｀Д´)ﾉ`, `(ノಠ益ಠ)ノ`
6. **Confused** (困惑) - `(・・?`, `(｡•́︿•̀｡)`, `ヽ(´ー｀)ﾉ`
7. **Surprised** (驚き) - `(⊙_⊙)`, `(°ロ°)`, `Σ(ﾟДﾟ)`
8. **Cute** (かわいい) - `(◕‿◕)`, `(✿◠‿◠)`, `(◠‿◠✿)`
9. **Animals** (動物) - `(=^･ω･^=)`, `U•ﻌ•U`, `(◕‿◕✿)`
10. **Actions** (動作) - `(^_^)ゞ`, `m(_ _)m`, `ヾ(^-^)ノ`
11. **Objects** (物) - `♪♫`, `☆`, `✿`
12. **Symbols** (記号) - `→`, `↑`, `✓`

### Usage

**Toggle Picker:**
```javascript
function toggleKaomoji() {
    const picker = document.getElementById('kaomoji-picker');
    picker.style.display = picker.style.display === 'none' ? 'block' : 'none';
}
```

**Insert Emoticon:**
```javascript
function insertKaomoji(kaomoji, fieldId) {
    const field = document.getElementById(fieldId);
    const cursorPos = field.selectionStart;
    const textBefore = field.value.substring(0, cursorPos);
    const textAfter = field.value.substring(cursorPos);
    field.value = textBefore + kaomoji + textAfter;
    field.focus();
    field.setSelectionRange(cursorPos + kaomoji.length, cursorPos + kaomoji.length);
}
```

### UI Design

**Picker Container:**
```css
.kaomoji-picker {
    border: 1px solid #ddd;
    border-radius: 4px;
    padding: 16px;
    background: white;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}
```

**Category Buttons:**
```css
.kaomoji-category-btn {
    padding: 8px 16px;
    margin: 4px;
    border: 1px solid #ddd;
    border-radius: 4px;
    cursor: pointer;
    transition: all 0.3s ease;
}
```

**Kaomoji Items:**
```css
.kaomoji-item {
    display: inline-block;
    padding: 8px;
    margin: 4px;
    cursor: pointer;
    border-radius: 4px;
    transition: background 0.2s;
}

.kaomoji-item:hover {
    background: #f0f0f0;
}
```

## Language Switcher

### Design

**Switcher Buttons:**
```css
.lang-switch {
    display: flex;
    gap: 8px;
    margin-bottom: 16px;
}

.lang-btn {
    padding: 8px 16px;
    border: 1px solid rgba(255,255,255,0.3);
    border-radius: 4px;
    cursor: pointer;
    transition: all 0.3s ease;
}

.lang-btn.active {
    background: rgba(255,255,255,0.2);
}
```

**Position:**
- Desktop: Top-right corner of header
- Mobile: Below header, full width

### Functionality

**Set Language:**
```javascript
function setLanguage(lang) {
    document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';
    const url = new URL(window.location.href);
    url.searchParams.set('lang', lang);
    window.location.href = url.toString();
}
```

## Accessibility

### Touch Targets

All interactive elements meet the minimum touch target size of 48x48 pixels:
- Buttons: `min-height: 48px`
- Links: `min-height: 48px` with adequate padding
- Form inputs: `min-height: 48px`

### Focus States

Clear focus indicators for keyboard navigation:
```css
button:focus, input:focus, textarea:focus, a:focus {
    outline: 2px solid #1976d2;
    outline-offset: 2px;
}
```

### Color Contrast

All text meets WCAG AA standards:
- Body text: `rgba(0,0,0,0.87)` on white (contrast ratio > 4.5:1)
- Secondary text: `rgba(0,0,0,0.54)` on white (contrast ratio > 4.5:1)
- Button text: White on `#1976d2` (contrast ratio > 4.5:1)

## Performance

### CSS Optimization

- Use of CSS variables for consistent theming
- Minimal use of animations (only on transitions)
- Hardware-accelerated transforms
- Efficient selectors

### JavaScript

- Event delegation for kaomoji picker
- Minimal DOM manipulation
- Efficient cursor position handling
- No external dependencies

## Browser Support

**Modern Browsers:**
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

**Features Used:**
- CSS Grid and Flexbox
- CSS Custom Properties (Variables)
- ES6 JavaScript
- Modern form inputs

## Future Enhancements

### Planned Features

1. **Dark Mode**
   - Toggle between light and dark themes
   - Respect system preferences
   - Persistent user choice

2. **Customizable Themes**
   - User-selectable color schemes
   - Custom accent colors
   - Theme presets

3. **Enhanced Kaomoji**
   - Search functionality
   - Favorite emoticons
   - Recent emoticons
   - Custom emoticons

4. **Improved Accessibility**
   - Screen reader support
   - High contrast mode
   - Font size adjustment
   - Keyboard shortcuts

5. **Progressive Web App (PWA)**
   - Offline support
   - Install to home screen
   - Push notifications
   - Background sync

## Related Documentation

- [LOCALIZATION.md](LOCALIZATION.md) - Internationalization details
- [REFACTORING_SUMMARY.md](REFACTORING_SUMMARY.md) - Code organization
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [README.md](README.md) - General project information
