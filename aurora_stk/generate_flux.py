from ovationpyme import ovation_prime, ovation_utilities
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import datetime, aacgmv2
import numpy as np
from scipy.interpolate import griddata, SmoothSphereBivariateSpline

def get_flux_geo(dt, atype="diff", jtype="electron energy flux"):
	estimator = ovation_prime.FluxEstimator(atype,jtype)

	mlatgrid,mltgrid,fluxgrid = estimator.get_flux_for_time(dt)
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
	cutoff = 15
	
	dt = datetime.datetime(2003,05,04,20,0,0)

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
	points = np.transpose([lon_grid.flatten(),lat_grid.flatten()])
	grid_lat, grid_lon = np.meshgrid(np.linspace(40,90,256),np.linspace(-180,180,400))
	grid_flux = griddata(points, fluxgrid.flatten(), (grid_lon, grid_lat), 
		method='cubic', fill_value=0)
	im_flux = np.transpose(grid_flux)
	for minv in [0.15]:
		# Create the brightness factor
		max_bright_flux = 2
		# flux_factor = np.transpose(grid_flux)/max_bright_flux #- subtract/256.
		# bound the flux factor to [0,1]
		# flux_factor[flux_factor>1] = 1
		# flux_factor[flux_factor<0] = 0

		#Create black image
		# out_im = np.zeros(flux_factor.shape + (3,),dtype=np.uint8)
		# define color to use
		green = (np.array([189,255,0])*1./256).reshape((1,3))
		colors=np.linspace(0,1,256).reshape((256,1))*green
		colors[:cutoff,:] = [0,0,0]
		cmap = ListedColormap(colors,
			name='aurora')

		sn = 'ovation_flux_{}_v{}.png'.format(dt.isoformat(),minv)
		plt.imsave(sn,im_flux,
			vmin=minv,vmax=max_bright_flux,
			cmap=cmap)

		print("Image saved to "+ sn)