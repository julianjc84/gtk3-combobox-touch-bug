# GTK3 ComboBox Touch Bug — Reproducer and Fix

## The Bug

GtkComboBox popups dismiss immediately on touchscreen tap in GTK3. This affects flat-list `GtkComboBox` (with `GtkListStore` model) and `GtkComboBoxText`. Tree-based (submenu) and entry-based combo boxes are not affected.

The bug is also dependent on `GtkScrolledWindow` scroll state — when content fits without scrolling (scrollbar hidden), all combobox popups dismiss on touch. When the window is resized small enough to require scrolling, they work correctly.

## What We Found

- Touchscreens generate `MotionNotify` events even without actual finger movement. GTK3 treats these as drag/dismiss gestures and immediately closes the combobox popup.
- The presence of an active scrollbar in a parent `GtkScrolledWindow` changes the event propagation path enough to prevent the dismissal.
- The bug exists in GTK's own demo app (`gtk3-demo --run combobox`) — "Items with icons" and "String IDs" fail on touch, while "Where are we?" (tree/submenu) and "Editable" (has-entry) work.
- The bug does **not** exist in GTK4. GTK4's `GtkDropDown` uses `GtkPopover` instead of GTK3's popup mechanism and handles touch correctly.

## The Fix

Wrapping comboboxes in a `GtkScrolledWindow` with vertical scroll policy set to `GTK_POLICY_ALWAYS` prevents the bug. The scrollbar is always present regardless of content size, which changes how touch events are propagated.

## Tested With

- GTK 3.24.41
- GTK 4.14.5
- Linux Mint / Ubuntu (Linux 6.x)
- Touchscreen input (not mouse — mouse works fine in all cases)

## Build

```
sudo apt install libgtk-3-dev libgtk-4-dev
make
```

## Test Apps

### ScrolledWindow Reproducer

Demonstrates the scroll-state dependency of the bug.

| App | Description |
|-----|-------------|
| `./combobox-touch-test-gtk3` | GTK3 — 3 tabs with different scroll policies |
| `./dropdown-touch-test-gtk4` | GTK4 — same layout using GtkDropDown (bug not present) |

**GTK3 tabs:**

| Tab | ComboBox Type | Scroll Policy | Touch |
|-----|--------------|---------------|-------|
| 1 | GtkComboBox + GtkListStore | AUTOMATIC | **BUG** — dismisses immediately |
| 2 | GtkComboBoxText | AUTOMATIC | **BUG** — dismisses immediately |
| 3 | GtkComboBoxText | ALWAYS | **WORKS** |

**Steps to reproduce:**

1. Run `./combobox-touch-test-gtk3` on a touchscreen
2. With the window large (no scrollbar visible), tap any combobox on Tab 1 or Tab 2
   - **Expected:** popup opens and stays open for selection
   - **Actual:** popup appears and immediately dismisses
3. Resize the window smaller until a scrollbar appears, then tap the same combobox
   - Popup now stays open and works correctly
4. Switch to Tab 3 (scroll policy forced to ALWAYS)
   - Comboboxes work correctly with touch at any window size

### gtk3-demo Combobox — Original vs Fixed

Standalone versions of GTK's own combobox demo, showing the bug and the fix side by side.

| App | Description |
|-----|-------------|
| `./gtk3-demo-combobox-original` | Unmodified — "Items with icons" and "String IDs" **fail** on touch |
| `./gtk3-demo-combobox-fixed` | Wrapped in GtkScrolledWindow (POLICY_ALWAYS) — all comboboxes **work** |

**Original demo touch results:**

| Section | Type | Touch |
|---------|------|-------|
| Items with icons | GtkComboBox + GtkListStore (flat) | **FAILS** |
| Where are we? | GtkComboBox + GtkTreeStore (tree/submenu) | Works |
| Editable | GtkComboBoxText with has-entry | Works |
| String IDs | GtkComboBoxText (flat) | **FAILS** |

### GTK4 Test

The GTK4 app uses `GtkDropDown` + `GtkStringList` with the same three-tab layout.

**The bug does not exist in GTK4.** All tabs work with touch regardless of scroll state. GTK4's touch interaction requires tap to open, tap to select, tap to confirm — this is normal behavior, not a bug.
