# APK-Sorter
This program renames and sorts APKs into directory structure that's arranged by APK name and version number. It runs only on Linux.

INSTALLATION:
1. Compile APK_Renamer.c
2. Download and compile AXMLDEC https://github.com/ytsutano/axmldec.
3. Move the compiled AXMLDEC binary into the same folder as the APK_Renamer binary.
4. Run the APK_Renamer binary from the same folder wherin it is located.

COMMAND LINE OPTIONS:

-r="PATH" - This option tells APK_Renamer where to find the APKs that are to be sorted. Replace PATH with the path of the folder containing the APKs.
-o="PATH" - This option tells APK_Renamer where to output the processed APKs. 
-l="NAME" - This options tells APK_Renmaer to create a log file. Replace NAME with the desired name for the log file.
