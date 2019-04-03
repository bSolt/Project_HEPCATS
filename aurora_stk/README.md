# Aurora-STK
## Project HEPCATS, CU-Boulder 2019
This file contains the code for using the [OvationPyme API](https://github.com/lkilcommons/OvationPyme) to generate Images of a simulated aurora Borealis in equirectangular, geographic latitude and longitude coordinate frame. 

The source code for this is within the file `generate_flux.py`. The code all electron types of aurora and uses `electron energy flux` output from the model to inform the brightness of the pixels in the output image. The following specifications are made in the code:
	Consider the range of fluxes from 0.1<=E<=4, this range is split into 2^8 colorbins. The colormap specifies that the brightness will increase quadtatically.