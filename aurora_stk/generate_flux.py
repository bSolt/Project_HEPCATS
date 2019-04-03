from __future__  import print_function
from ovationpyme import ovation_prime, ovation_utilities
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import datetime, aacgmv2
import numpy as np
from scipy.interpolate import griddata, SmoothSphereBivariateSpline

def get_flux_geo(dt):
	atypes=['diff','mono','wave']
	jtype='electron energy flux'
	fluxgrid = None
	# Iterate over types
	for atype in atypes:
		print("Generating fluxes for {} type aurora".format(atype),end="\r")
		# create estimator
		estimator = ovation_prime.FluxEstimator(atype,jtype)
		mlatgrid,mltgrid,fg = estimator.get_flux_for_time(dt)
		# Create fluxgrid in the first iteration
		if fluxgrid is None:
			fluxgrid = np.zeros(fg.shape)
		# Add to flux
		fluxgrid += fg
	print("Converting to GEO coordinates", end="\r")
	# Convert mag local time to magnetic longitude
	mlongrid = aacgmv2.convert_mlt(mltgrid,dt,m2a=True)
	# Convert CGM to GEO
	# altitude of aurora ~ 300 km
	aur_z = 300
	lat_grid, lon_grid, r_grid = aacgmv2.convert_latlon_arr(mlatgrid,mlongrid, #inputs
		aur_z,dt,u'A2G') # aurora height, datetime, and conversion mode
	return lat_grid, lon_grid, fluxgrid

# Main program
if __name__ == "__main__":
	# make all bins below cutoff black
	cutoff = 0
	#NOTE: flux values are erg/cm^2/s
	# bottom flux value
	minv = 0.1
	# max brightness flux value
	max_bright_flux = 4

	# define color to use
	green = (np.array([189,255,0])*1./256).reshape((1,3))
	colors=(np.linspace(0,1,256)**2).reshape((256,1))*green
	cmap = ListedColormap(colors,
			name='aurora')

	
	for hour in np.arange(0,24,0.25): # Iterate every 15
		dt = datetime.datetime(2003,10,29,0,0,0) + datetime.timedelta(hours=hour)
		# Get the raw data
		lat_grid, lon_grid, fluxgrid = get_flux_geo(dt)

		# Convert to finer spherical grid
		# Set up the spline interpolator
		# deg = np.pi/180
		# spline = SmoothSphereBivariateSpline(
		# 	deg*lat_grid.flatten(), #theta
		# 	deg*lon_grid.flatten()+np.pi, # phi
		# 	fluxgrid.flatten(),s=3.5)
		# scale_lat = np.linspace(40*deg,90*deg,256); scale_lon = np.linspace(-180*deg,180*deg,400)+np.pi;
		# grid_flux = spline(scale_lat,scale_lon)

		# Convert to known GEO grid
		print("Interpolating to output grid", end='\r')
		# create list of coordnation
		points = np.transpose([lon_grid.flatten(),lat_grid.flatten()])
		# create desired grid coordinates
		grid_lat, grid_lon = np.meshgrid(np.linspace(40,90,256),np.linspace(-180,180,400))
		# interpolate data onto new grid
		grid_flux = griddata(points, fluxgrid.flatten(), (grid_lon, grid_lat), 
			method='cubic', fill_value=0)
		# transpose so the image is the right direction
		im_flux = np.transpose(grid_flux)
		# Generate save name
		sn = 'ovation_flux_{}.png'.format(dt.isoformat()).replace(':','_')
		# Save image with chosen colors and 
		plt.imsave(sn,im_flux,
			vmin=minv,vmax=max_bright_flux,
			cmap=cmap)

		print("Image saved to "+ sn)