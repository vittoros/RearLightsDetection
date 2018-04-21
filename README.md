# RearLightsDetection
Implementation of a vehicle detection system, using a single camera, mounted in the front of the vehicle. The vehicles are 
located by spotting the rear red lights.

## Screenshots of full detection
![Alt text](/../readmeImages/readmeImages/pjimage2.jpg?raw=true)

## Examples of the Subsystems used
### Red Light detection subsystem
![Alt text](/../readmeImages/readmeImages/pjimage.jpg?raw=true)
**First row**: Original image and a* subchannel of Lab color scheme,
**Second row**: FRST pseudocolored and FRST adjusted to 0-255 range,
**Last row**: Otsu’s Threshold

### Connected Component Labelling
Example of the two-pass algorithm. Original image on the left
![Alt text](/../readmeImages/readmeImages/world.jpg?raw=true)

### Ellipse Fitting
![Alt text](/../readmeImages/readmeImages/ellipse.jpg?raw=true)

### Canny edge detector
![Alt text](/../readmeImages/readmeImages/R2D2.jpg?raw=true)
