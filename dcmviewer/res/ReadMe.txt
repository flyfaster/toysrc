ACKNOWLEDGEMENTS
1.This program uses dicomlib toolkit(http://dicomlib.swri.ca/).
2.Many thanks to my wife Echoef who encouraged me all the time.


Features
View uncompressed dicom images in full size or fit to window modes
CR/CT/MR/US/RT
support multiframe, use LEFT ARROW, UP ARROW, PAGE UP keys navigate to previous frame, 
      use RIGHT ARROW, DOWN ARROW, PAGE DOWN navigate to next frame
support window level adjustment via right mouse button moving
view DICOM header
View histogram of the image(use LEFT ARROW, RIGHT ARROW key to move the mark line, 
    or draging right mouse button to move the mark line)
Accept dropping DICOM files into the main window
Support printing to one page.
Invert image display from menu
Pop up adjust window level dialog from menu
Can work as C-STORE SCP, accept image from C-STORE SCU ( Dont care AE)
Can work as C-ECHO SCP.
Can work as C-MOVE SCU, use button "Retrieve" to get images.
Can work as C-FIND SCU, use "Search" button to query Archive Server. Use "Server" button to configure other AEs
Convert RGB DICOM file as grey DICOM file (with the same UID). ( grey = (R+G+B)/3 ) Warning, This conversion is invalid for diagnosis!
Save DICOM as BMP/JPEG/GIF/TIFF/PNG image
Save DICOM header as text file
can open .txt file 

limitation:
can not open compressed files

The annotation toolbar does not work. This release fix a bug, so I publish it before finishing the annotation.
Free for personal noncommercial use only. 
No distribution without the author's agreement.
Use it at your own risk. You can report bug.  
Sponsor is welcome for enhancement.

www.fruitfruit.com
20060507
