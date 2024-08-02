# OBS Status Bar Plugin

This plugin allows you to display custom text information in the OBS Studio status bar. 

## Features

* **Dynamic Updates:** Easily update the displayed information by adding or modifying `.txt` files in the designated plugin folder.
* **Real-Time Refreshing:** The status bar updates automatically whenever a `.txt` file is added, removed, or changed. 

## Usage

1. Install the plugin.
2. Create `.txt` files with the desired text content. 
3. Place the `.txt` files in the designated plugin folder (the plugin will create this folder automatically upon first launch).
4. The text from your `.txt` files will now appear in the OBS status bar.

## Example

Create a file named `streaming_info.txt` with the content:

```
Stream Status: Live!
```

Place this file in the plugin folder. You should now see "Stream Status: Live!" displayed in the OBS status bar. 

**Note:** The content of each `.txt` file will appear as a separate label in the status bar. 
